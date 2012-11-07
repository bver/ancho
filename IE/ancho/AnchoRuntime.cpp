/****************************************************************************
 * AnchoRuntime.cpp : Implementation of CAnchoRuntime
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include <map>

#include "ProtocolCF.h"

#include "AnchoRuntime.h"
#include "AnchoAddon.h"
#include "AnchoBrowserEvents.h"
#include "AnchoPassthruAPP.h"
#include "dllmain.h"

extern class CanchoModule _AtlModule;

typedef PassthroughAPP::CMetaFactory<PassthroughAPP::CComClassFactoryProtocol,
  CAnchoPassthruAPP> MetaFactory;

/*============================================================================
 * class CAnchoRuntime
 */

//----------------------------------------------------------------------------
//  InitAddons
HRESULT CAnchoRuntime::InitAddons()
{
  // create all addons
  // open the registry key where all extensions are registered,
  // iterate subkeys and load each extension

  CRegKey regKey;
  LONG res = regKey.Open(HKEY_CURRENT_USER, s_AnchoExtensionsRegistryKey, KEY_READ);
  if (ERROR_SUCCESS != res)
  {
    return HRESULT_FROM_WIN32(res);
  }
  DWORD iIndex = 0;
  CString sKeyName;
  DWORD dwLen = 4096;
  HRESULT hr = S_OK;

  while(ERROR_SUCCESS == regKey.EnumKey(iIndex++, sKeyName.GetBuffer(dwLen), &dwLen, NULL))
  {
    sKeyName.ReleaseBuffer();
    CAnchoAddonComObject * pNewObject = NULL;
    hr = CAnchoAddonComObject::CreateInstance(&pNewObject);
    if (SUCCEEDED(hr))
    {
      CComPtr<IAnchoAddon> addon(pNewObject);
      hr = addon->Init(sKeyName, m_pAnchoService, m_pWebBrowser);
      if (SUCCEEDED(hr))
      {
        m_Addons[sKeyName] = addon;
      }
    }
    dwLen = 4096;
  }

  return S_OK;
}

//----------------------------------------------------------------------------
//  DestroyAddons
void CAnchoRuntime::DestroyAddons()
{
  CString sAddonID;
  POSITION pos = m_Addons.GetStartPosition();
  while(pos)
  {
    m_Addons.GetNextValue(pos)->Shutdown();
  }
  m_Addons.RemoveAll();
  m_pAnchoService.Release();
  m_pWebBrowser.Release();
}

//----------------------------------------------------------------------------
//  DestroyAddons
HRESULT CAnchoRuntime::DeinitBrowserEventSource()
{
  AtlUnadvise(m_pWebBrowser, DIID_DWebBrowserEvents2, m_WebBrowserEventsCookie);
  AtlUnadvise(m_pBrowserEventSource, IID_DAnchoBrowserEvents, m_AnchoBrowserEventsCookie);

  return S_OK;
}

//----------------------------------------------------------------------------
//  InitBrowserEventSink
HRESULT CAnchoRuntime::InitBrowserEventSource()
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

  AtlAdvise(m_pWebBrowser, (IUnknown *)(TWebBrowserEvents *) this, DIID_DWebBrowserEvents2, &m_WebBrowserEventsCookie);

  // create addon service object
  IF_FAILED_RET(m_pAnchoService.CoCreateInstance(CLSID_AnchoAddonService));

  CComObject<CAnchoBrowserEvents>* pBrowserEventSource;
  HRESULT hr = CComObject<CAnchoBrowserEvents>::CreateInstance(&pBrowserEventSource);
  IF_FAILED_RET(hr);

  m_pBrowserEventSource = pBrowserEventSource;
  
  AtlAdvise(m_pBrowserEventSource, (IUnknown*)(TAnchoBrowserEvents*) this, IID_DAnchoBrowserEvents,
    &m_AnchoBrowserEventsCookie);

  // Set the sink as property of the browser so it can be retrieved if someone wants to send
  // us events.
  hr = m_pWebBrowser->PutProperty(L"_anchoBrowserEvents", CComVariant(m_pBrowserEventSource));

  return hr;
}

//----------------------------------------------------------------------------
//  OnBrowserBeforeNavigate2
STDMETHODIMP_(void) CAnchoRuntime::OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags,
  VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel)
{
  ATLASSERT(pURL->vt == VT_BSTR && pURL->bstrVal != NULL);
  CComQIPtr<IWebBrowser2> webBrowser(pDisp);
  ATLASSERT(webBrowser != NULL);
  CComBSTR url;
  removeUrlFragment(pURL->bstrVal, &url);
  m_Frames[(BSTR) url] = webBrowser;
}

//----------------------------------------------------------------------------
//  OnFrameStart
STDMETHODIMP CAnchoRuntime::OnFrameStart(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame)
{
  return ApplyContentScripts(bstrUrl, bIsMainFrame, L"start");
}

//----------------------------------------------------------------------------
//  OnFrameEnd
STDMETHODIMP CAnchoRuntime::OnFrameEnd(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame)
{
  return ApplyContentScripts(bstrUrl, bIsMainFrame, L"end");
}

//----------------------------------------------------------------------------
//  OnFrameRedirect
STDMETHODIMP CAnchoRuntime::OnFrameRedirect(BSTR bstrOldUrl, BSTR bstrNewUrl)
{
  CComBSTR url;
  removeUrlFragment(bstrOldUrl, &url);
  std::map<std::wstring, CComPtr<IWebBrowser2> >::iterator it = m_Frames.find(bstrOldUrl);
  if (it != m_Frames.end()) {
    removeUrlFragment(bstrNewUrl, &url);
    m_Frames[(BSTR) url] = it->second;
    m_Frames.erase(it);
  }
  return S_OK;
}

//----------------------------------------------------------------------------
//  ApplyContentScripts
HRESULT CAnchoRuntime::ApplyContentScripts(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame, BSTR bstrPhase)
{
  CComPtr<IWebBrowser2> webBrowser;
  if (bIsMainFrame) {
    webBrowser = m_pWebBrowser;
  }
  else {
    CComBSTR url;
    removeUrlFragment(bstrUrl, &url);
    std::map<std::wstring, CComPtr<IWebBrowser2> >::iterator it = m_Frames.find((BSTR) url);
    if (it == m_Frames.end()) {
      ATLASSERT(CComBSTR("end") == bstrPhase);
      // Assume we have removed this frame while waiting for the document to complete.
      return S_FALSE;
    }
    webBrowser = it->second;
  }
  // We have to check whether we are the main web browser since the event source can't
  // always tell us if we are the main frame.
  if ((bIsMainFrame || webBrowser == m_pWebBrowser) && (CComBSTR(L"start") == bstrPhase)) {
    m_Frames.clear();
  }
  CString sAddonID;
  POSITION pos = m_Addons.GetStartPosition();
  while(pos) {
    m_Addons.GetNextValue(pos)->ApplyContentScripts(webBrowser, bstrUrl, bstrPhase);
  }

  return S_OK;
}

//----------------------------------------------------------------------------
//  SetSite
STDMETHODIMP CAnchoRuntime::SetSite(IUnknown *pUnkSite)
{
  CComPtr<IInternetSession> pInternetSession;
  IF_FAILED_RET(CoInternetGetSession(0, &pInternetSession, 0));

  IF_FAILED_RET(MetaFactory::CreateInstance(CLSID_HttpProtocol, &m_CFHTTP));
  IF_FAILED_RET(pInternetSession->RegisterNameSpace(m_CFHTTP, CLSID_NULL, L"http", 0, 0, 0));

  IF_FAILED_RET(MetaFactory::CreateInstance(CLSID_HttpSProtocol, &m_CFHTTPS));
  IF_FAILED_RET(pInternetSession->RegisterNameSpace(m_CFHTTPS, CLSID_NULL, L"https", 0, 0, 0));

  HRESULT hr = IObjectWithSiteImpl<CAnchoRuntime>::SetSite(pUnkSite);
  IF_FAILED_RET(hr);
  if (pUnkSite)
  {
    hr = InitBrowserEventSource();
    if (SUCCEEDED(hr)) {
      hr = InitAddons();
    }
  }
  else
  {
    DestroyAddons();
  }
  return hr;
}
