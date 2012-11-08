/****************************************************************************
 * AnchoAddon.cpp : Implementation of CAnchoAddon
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoAddon.h"
#include "dllmain.h"

extern class CanchoModule _AtlModule;

/*============================================================================
 * class CAnchoAddon
 */

//----------------------------------------------------------------------------
//  Init
STDMETHODIMP CAnchoAddon::Init(LPCOLESTR lpsExtensionID, IAnchoAddonService * pService, IWebBrowser2 * pWebBrowser)
{
  m_pWebBrowser = pWebBrowser;
  m_pAnchoService = pService;
  m_sExtensionName = lpsExtensionID;

  // lookup ID in registry
  CRegKey regKey;
  CString sKey;
  sKey.Format(_T("%s\\%s"), s_AnchoExtensionsRegistryKey, m_sExtensionName);
  LONG res = regKey.Open(HKEY_CURRENT_USER, sKey, KEY_READ);
  if (ERROR_SUCCESS != res)
  {
    return HRESULT_FROM_WIN32(res);
  }

  // get addon GUID
  ULONG nChars = 37;  // length of a GUID + terminator
  res = regKey.QueryStringValue(s_AnchoExtensionsRegistryEntryGUID, m_sExtensionID.GetBuffer(nChars), &nChars);
  m_sExtensionID.ReleaseBuffer();
  if (ERROR_SUCCESS != res)
  {
    return HRESULT_FROM_WIN32(res);
  }

  // get addon path
  nChars = _MAX_PATH;
  LPTSTR pst = m_sExtensionPath.GetBuffer(nChars+1);
  res = regKey.QueryStringValue(s_AnchoExtensionsRegistryEntryPath, pst, &nChars);
  pst[nChars] = 0;
  PathAddBackslash(pst);
  m_sExtensionPath.ReleaseBuffer();
  if (ERROR_SUCCESS != res)
  {
    return HRESULT_FROM_WIN32(res);
  }
  if (!PathIsDirectory(m_sExtensionPath))
  {
    return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
  }

  // advise DWebBrowserEvents2
  ATLASSERT(!m_dwAdviseSinkWebBrowser);
  AtlAdvise(m_pWebBrowser, (IUnknown*)(DWebBrowserEvents2Ancho*)this, DIID_DWebBrowserEvents2, &m_dwAdviseSinkWebBrowser);

  // The addon can be a resource DLL or simply a folder in the filesystem.
  // TODO: Load the DLL if there is any.

  // create content script engine
#ifdef MAGPIE_REGISTERED
  IF_FAILED_RET(m_Magpie.CoCreateInstance(CLSID_MagpieApplication));
#else
  CComBSTR bs;
  IF_FAILED_RET(m_pAnchoService->GetModulePath(&bs));

  // Load magpie from the same path where this exe file is.
  CString s(bs);
  s += _T("Magpie.dll");
  HMODULE hModMagpie = ::LoadLibrary(s);
  if (!hModMagpie)
  {
    return E_FAIL;
  }
  fnCreateMagpieInstance CreateMagpieInstance = (fnCreateMagpieInstance)::GetProcAddress(hModMagpie, "CreateMagpieInstance");
  if (!CreateMagpieInstance)
  {
    return E_FAIL;
  }
  IF_FAILED_RET(CreateMagpieInstance(&m_Magpie));
#endif

  return S_OK;
}

//----------------------------------------------------------------------------
//  Shutdown
STDMETHODIMP CAnchoAddon::Shutdown()
{
  // this method must be safe to be called multiple times
  m_pContentAPI.Release();
  m_pBackgroundConsole.Release();
  m_Magpie.Release();

  if (m_pAddonBackground)
  {
    m_pAddonBackground->UnadviseInstance(m_InstanceID);
  }
  m_pAddonBackground.Release();

  m_pAnchoService.Release();

  if (m_pWebBrowser)
  {
    if (m_dwAdviseSinkWebBrowser)
    {
      AtlUnadvise(m_pWebBrowser, DIID_DWebBrowserEvents2, m_dwAdviseSinkWebBrowser);
      m_dwAdviseSinkWebBrowser = 0;
    }
    m_pWebBrowser.Release();
  }
  return S_OK;
}
//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddon::executeScriptCode(BSTR aCode)
{
  //TODO: it needs method which is not implmented in magpie yet
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddon::executeScriptFile(BSTR aFile)
{
  //TODO: improve, when manifest processing finished
  return m_Magpie->Run(aFile);
}

//----------------------------------------------------------------------------
//  BrowserNavigateCompleteEvent
STDMETHODIMP_(void) CAnchoAddon::BrowserNavigateCompleteEvent(IDispatch *pDisp, VARIANT *URL)
{
  //If create AddonBackground sooner - background script will be executed before initialization of tab windows
  if(!m_pAddonBackground) {
    if(S_OK != m_pAnchoService->GetAddonBackground(CComBSTR(m_sExtensionName), &m_pAddonBackground)) {
      ATLTRACE(L"Ancho addon couldn't acquire reference to AddonBackground\n");
      return;
    }

    // get console
    m_pBackgroundConsole = m_pAddonBackground;
    ATLASSERT(m_pBackgroundConsole);

    // tell background we are there and get instance id
    m_pAddonBackground->AdviseInstance(&m_InstanceID);

    // get content our API
    m_pAddonBackground->GetContentAPI(m_InstanceID, &m_pContentAPI);
  }

  // content script handling happens here

  // no frame handling
  // TODO: decide how to handle frames
  if (!m_pWebBrowser.IsEqualObject(pDisp))
  {
    // pDisp is the webbrowser control of a frame
    return;
  }

  if (!m_pContentAPI)
  {
    return;
  }

  // TODO: URL matching
  // (re)initialize magpie for this page
  m_Magpie->Shutdown();
  CString s;
  s.Format(_T("Ancho content [%s]"), m_sExtensionName);
  HRESULT hr = m_Magpie->Init((LPWSTR)(LPCWSTR)s);
  if (FAILED(hr))
  {
    return;
  }

  // add a loader for scripts in the extension filesystem
  hr = m_Magpie->AddFilesystemScriptLoader((LPWSTR)(LPCWSTR)m_sExtensionPath);
  if (FAILED(hr))
  {
    return;
  }

  // inject items: chrome, console and window with global members
//  CComQIPtr<IDispatch> pDispConsole;
  CComQIPtr<IWebBrowser2> pWebBrowser(pDisp);
  CIDispatchHelper script = CIDispatchHelper::GetScriptDispatch(pWebBrowser);
  if (!script)
  {
    return;
  }

  m_Magpie->AddNamedItem(L"chrome", m_pContentAPI, SCRIPTITEM_ISVISIBLE|SCRIPTITEM_CODEONLY);
  //m_Magpie->AddNamedItem(L"console", pDispConsole, SCRIPTITEM_ISVISIBLE|SCRIPTITEM_CODEONLY);
  m_Magpie->AddNamedItem(L"window", script.p, SCRIPTITEM_ISVISIBLE|SCRIPTITEM_GLOBALMEMBERS);

  // TODO: get the name(s) of content scripts from manifest and run them in order
  //       for now we run a hardcoded script "content.js"
  // and run content script
  m_Magpie->Run(L"content.js");
}
