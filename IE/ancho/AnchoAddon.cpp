// AnchoAddon.cpp : Implementation of CAnchoAddon

#include "stdafx.h"
#include "AnchoAddon.h"
#include "dllmain.h"


// CAnchoAddon
extern class CanchoModule _AtlModule;

// TODO: this has to go into the registry somehow
LPCWSTR CAnchoAddon::s_ExtensionName = L"ancho-test.seiberspace.de";

STDMETHODIMP CAnchoAddon::SetSite(IUnknown *pUnkSite)
{
  HRESULT hr = IObjectWithSiteImpl<CAnchoAddon>::SetSite(pUnkSite);
  IF_FAILED_RET(hr);
  if (pUnkSite)
  {
    hr = InitAddon();
  }
  else
  {
    DestroyAddon();
  }
	return hr;
}

HRESULT CAnchoAddon::InitAddon()
{
  HRESULT hrRet = E_FAIL;
  do
  {
    ATLASSERT(m_spUnkSite);
    // get IServiceProvider to get IWebBrowser2 and IOleWindow
    CComQIPtr<IServiceProvider> pServiceProvider = m_spUnkSite;
    if (!pServiceProvider)
    {
      return E_FAIL;
    }

    // get IWebBrowser2
    pServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser.p);
    if (!m_pWebBrowser)
    {
      return E_FAIL;
    }

    // advise DWebBrowserEvents2
    ATLASSERT(!m_dwAdviseSinkWebBrowser);
    AtlAdvise(m_pWebBrowser, (IUnknown*)(DWebBrowserEvents2Ancho*)this, DIID_DWebBrowserEvents2, &m_dwAdviseSinkWebBrowser);

    // create scripting service object
    IF_FAILED_BREAK(m_pAnchoService.CoCreateInstance(CLSID_AnchoAddonService), hrRet);

    // get addon instance
    IF_FAILED_BREAK(m_pAnchoService->GetExtension(CComBSTR(s_ExtensionName), &m_pAddonBackground), hrRet);

    // The addon can be a resource DLL or simply a folder in the filesystem.
    // TODO: Load the DLL if there is any.

    // create content script engine
  #ifdef MAGPIE_REGISTERED
    IF_FAILED_BREAK(m_Magpie.CoCreateInstance(CLSID_MagpieApplication), hrRet);
  #else
    CComBSTR bs;
    IF_FAILED_BREAK(m_pAnchoService->GetModulePath(&bs), hrRet);

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
    IF_FAILED_BREAK(CreateMagpieInstance(&m_Magpie), hrRet);
  #endif
    // TODO: handle resource DLLs
  /*
    // add a loader for scripts in the extension filesystem
    IF_FAILED_BREAK(m_Magpie->AddFilesystemScriptLoader((LPWSTR)(LPCWSTR)sPath), hrRet);

    // add a loder for scripts in this exe file
    IF_FAILED_BREAK(m_Magpie->AddResourceScriptLoader((ULONG)_AtlModule.GetResourceHandle()), hrRet);

    // advise logger
    IF_FAILED_BREAK(AtlAdvise(m_Magpie, dynamic_cast<IUnknown*>(dynamic_cast<CAnchoAddonBackgroundLogger*>(this)), DIID__IMagpieLoggerEvents, &m_dwMagpieSinkCookie), hrRet);
  */

    // get console
    m_pBackgroundConsole = m_pAddonBackground;
    ATLASSERT(m_pBackgroundConsole);

    // get content API
    IF_FAILED_BREAK(m_pAddonBackground->AdviseInstance(&m_InstanceID), hrRet);
    IF_FAILED_BREAK(m_pAddonBackground->GetContentAPI(m_InstanceID, &m_pContentAPI), hrRet);

    hrRet = S_OK;
  } while(0);
  if (FAILED(hrRet))
  {
    DestroyAddon();
  }
  return hrRet;
}

void CAnchoAddon::DestroyAddon()
{
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
}


STDMETHODIMP_(void) CAnchoAddon::BrowserNavigateCompleteEvent(IDispatch *pDisp, VARIANT *URL)
{
  // content script handling happens here
  // 
  // if URL matches:
  // get the content API
  // inject content API
  // load content scripts
  // inject window object into magpie
/*
  if (!m_pMyObject)
    return;
  // get content API
  CComPtr<IDispatch> pContentAPI;
  HRESULT hr = m_pMyObject->GetContentAPI(&pContentAPI);
  if (FAILED(hr))
    return;

  CIDispatchHelper script = CIDispatchHelper::GetScriptDispatch(m_pWebBrowser);
  if (!script)
    return;
  hr = script.SetProperty(L"ancho", CComVariant(pContentAPI));
*/
  int asd = 0;
}

