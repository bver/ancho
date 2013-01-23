/****************************************************************************
 * AnchoAddon.cpp : Implementation of CAnchoAddon
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoAddon.h"
#include "dllmain.h"
#include "ProtocolHandlerRegistrar.h"

/*============================================================================
 * class CAnchoAddon
 */

//----------------------------------------------------------------------------
//  Init
STDMETHODIMP CAnchoAddon::Init(LPCOLESTR lpsExtensionID, IAnchoAddonService * pService,
  IWebBrowser2 * pWebBrowser)
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

  // load flags to see if the addon is disabled
  DWORD flags = 0;
  res = regKey.QueryDWORDValue(s_AnchoExtensionsRegistryEntryFlags, flags);
  // to stay compatible with older versions we treat "no flags at all" as "enabled"
  if ( (ERROR_SUCCESS == res) && !(flags & ENABLED))
  {
    // ... means: only when flag is present AND ENABLED is not set
    // the addon is disabled
    return E_ABORT;
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

  IF_FAILED_RET(CProtocolHandlerRegistrar::
    RegisterTemporaryFolderHandler(s_AnchoProtocolHandlerScheme, m_sExtensionName, m_sExtensionPath));

  // get addon instance
  //IF_FAILED_RET(m_pAnchoService->GetAddonBackground(CComBSTR(m_sExtensionName), &m_pAddonBackground));

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
  cleanupScripting();
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
  ATLASSERT(m_Magpie);
  return m_Magpie->Run(aFile);
}

//----------------------------------------------------------------------------
//  CleanupContentScripting
void CAnchoAddon::cleanupScripting()
{
  if (m_Magpie) {
    m_Magpie->Shutdown();
  }
  if (m_wrappedWindow) {
    m_wrappedWindow->cleanup();
  }
  m_wrappedWindow.Release();

  if (m_pAddonBackground && m_InstanceID) {
    m_pAddonBackground->ReleaseContentInfo(m_InstanceID);
  }
  if (m_pContentInfo) {
    m_pContentInfo.Release();
  }
}

//----------------------------------------------------------------------------
//  InitializeContentScripting
STDMETHODIMP CAnchoAddon::InitializeContentScripting(IWebBrowser2* pBrowser, BSTR bstrUrl, documentLoadPhase aPhase)
{
  IF_FAILED_RET(initializeEnvironment());

  // content script handling happens here

  // no frame handling
  // TODO: decide how to handle frames
  if (!m_pWebBrowser.IsEqualObject(pBrowser)) {
    return S_OK;
  }

  if (aPhase != documentLoadEnd) {
    return S_OK;
  }

  cleanupScripting();

  // get content our API
  IF_FAILED_RET(m_pAddonBackground->GetContentInfo(m_InstanceID, bstrUrl, &m_pContentInfo));

  CString s;
  s.Format(_T("Ancho content [%s] [%i]"), m_sExtensionName, m_InstanceID);
  IF_FAILED_RET(m_Magpie->Init((LPWSTR)(LPCWSTR)s));

  // add a loader for scripts in the extension filesystem
  IF_FAILED_RET(m_Magpie->AddFilesystemScriptLoader((LPWSTR)(LPCWSTR)m_sExtensionPath));

  // inject items: chrome, console and window with global members
  CComQIPtr<IWebBrowser2> pWebBrowser(pBrowser);
  ATLASSERT(pWebBrowser);

  CIDispatchHelper contentInfo(m_pContentInfo);
  CComVariant jsObj;
  IF_FAILED_RET((contentInfo.Get<CComVariant, VT_DISPATCH, IDispatch*>(L"api", jsObj)));
  IF_FAILED_RET(DOMWindowWrapper::createInstance(pWebBrowser, m_wrappedWindow))
  m_Magpie->AddNamedItem(L"chrome", jsObj.pdispVal, SCRIPTITEM_ISVISIBLE|SCRIPTITEM_CODEONLY);
  m_Magpie->AddNamedItem(L"window", m_wrappedWindow, SCRIPTITEM_ISVISIBLE|SCRIPTITEM_GLOBALMEMBERS);

  CIDispatchHelper window = m_wrappedWindow;
  CComPtr<IDispatchEx> pRequest;
  IF_FAILED_RET(pRequest.CoCreateInstance(__uuidof(AnchoXmlHttpRequest)));

  IF_FAILED_RET(window.SetProperty((LPOLESTR)L"XMLHttpRequest", CComVariant(pRequest.p)));
  m_Magpie->AddNamedItem(L"XMLHttpRequest", pRequest, SCRIPTITEM_ISVISIBLE|SCRIPTITEM_CODEONLY);


  // get the name(s) of content scripts from manifest and run them in order
  IF_FAILED_RET((contentInfo.Get<CComVariant, VT_DISPATCH, IDispatch*>(L"scripts", jsObj)));

  VariantVector scripts;
  IF_FAILED_RET(addJSArrayToVariantVector(jsObj.pdispVal, scripts));

  // scripts array is in reverse order here!
  for(VariantVector::reverse_iterator it = scripts.rbegin(); it != scripts.rend(); it++) {
    if( it->vt == VT_BSTR ) {
      m_Magpie->ExecuteGlobal(it->bstrVal);
    }
  }

  return S_OK;
}

//----------------------------------------------------------------------------
//  InitializeExtensionScripting
STDMETHODIMP CAnchoAddon::InitializeExtensionScripting(BSTR bstrUrl)
{
  IF_FAILED_RET(initializeEnvironment());

  cleanupScripting();

  // get content our API
  IF_FAILED_RET(m_pAddonBackground->GetContentInfo(m_InstanceID, bstrUrl, &m_pContentInfo));

  CIDispatchHelper contentInfo(m_pContentInfo);
  CComVariant api;
  IF_FAILED_RET((contentInfo.Get<CComVariant, VT_DISPATCH, IDispatch*>(L"api", api)));

  CIDispatchHelper script = CIDispatchHelper::GetScriptDispatch(m_pWebBrowser);
  IF_FAILED_RET(script.SetPropertyByRef(L"chrome", api));
  CComVariant console;
  IF_FAILED_RET((contentInfo.Get<CComVariant, VT_DISPATCH, IDispatch*>(L"console", console)));
  IF_FAILED_RET(script.SetPropertyByRef(L"console", console));

  return S_OK;
}

HRESULT CAnchoAddon::initializeEnvironment()
{
  //If create AddonBackground sooner - background script will be executed before initialization of tab windows
  if(!m_pAddonBackground || !m_pBackgroundConsole) {
    IF_FAILED_RET(m_pAnchoService->GetAddonBackground(CComBSTR(m_sExtensionName), &m_pAddonBackground));

    // get console
    m_pBackgroundConsole = m_pAddonBackground;
    ATLASSERT(m_pBackgroundConsole);
  }
  if(!m_InstanceID) {
    // tell background we are there and get instance id
    m_pAddonBackground->AdviseInstance(&m_InstanceID);

     //TODO - should be executed as soon as possible
    m_pAnchoService->webBrowserReady();
  }

  return S_OK;
}