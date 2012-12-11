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

#include <string>

#include <Iepmapi.h>
#pragma comment(lib, "Iepmapi.lib")

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
        m_Addons[std::wstring(sKeyName)] = addon;
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
  AddonMap::iterator it = m_Addons.begin();
  while(it != m_Addons.end()) {
    it->second->Shutdown();
    ++it;
  }
  m_Addons.clear();

  if(m_pAnchoService) {
    m_pAnchoService->unregisterRuntime(m_TabID);
  }
  m_pAnchoService.Release();
  if (m_pWebBrowser)
  {
    m_pWebBrowser.Release();
  }
}

//----------------------------------------------------------------------------
//  Cleanup
HRESULT CAnchoRuntime::Cleanup()
{
  AtlUnadvise(m_pWebBrowser, DIID_DWebBrowserEvents2, m_WebBrowserEventsCookie);
  AtlUnadvise(m_pBrowserEventSource, IID_DAnchoBrowserEvents, m_AnchoBrowserEventsCookie);

  return S_OK;
}

//----------------------------------------------------------------------------
//  Init
HRESULT CAnchoRuntime::Init()
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
  if (!m_pWebBrowser) {
    return E_FAIL;
  }

  AtlAdvise(m_pWebBrowser, (IUnknown *)(TWebBrowserEvents *) this, DIID_DWebBrowserEvents2, &m_WebBrowserEventsCookie);

  // create addon service object
  IF_FAILED_RET(m_pAnchoService.CoCreateInstance(CLSID_AnchoAddonService));

  // Registering tab in service - obtains tab id and assigns it to the tab as property
  IF_FAILED_RET(m_pAnchoService->registerRuntime(this, &m_TabID));
  HWND hwnd;
  m_pWebBrowser->get_HWND((long*)&hwnd);
  ::SetProp(hwnd, s_AnchoTabIDPropertyName, (HANDLE)m_TabID);

  CComObject<CAnchoBrowserEvents>* pBrowserEventSource;
  IF_FAILED_RET(CComObject<CAnchoBrowserEvents>::CreateInstance(&pBrowserEventSource));

  m_pBrowserEventSource = pBrowserEventSource;

  AtlAdvise(m_pBrowserEventSource, (IUnknown*)(TAnchoBrowserEvents*) this, IID_DAnchoBrowserEvents,
    &m_AnchoBrowserEventsCookie);

  // Set the sink as property of the browser so it can be retrieved if someone wants to send
  // us events.
  IF_FAILED_RET(m_pWebBrowser->PutProperty(L"_anchoBrowserEvents", CComVariant(m_pBrowserEventSource)));
  return S_OK;
}

//----------------------------------------------------------------------------
//  OnBrowserBeforeNavigate2
STDMETHODIMP_(void) CAnchoRuntime::OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags,
  VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel)
{
  // Add the frame to the frames map so we can retrieve the IWebBrowser2 object using the URL.
  ATLASSERT(pURL->vt == VT_BSTR && pURL->bstrVal != NULL);
  CComQIPtr<IWebBrowser2> pWebBrowser(pDisp);
  ATLASSERT(pWebBrowser != NULL);
  CComBSTR bstrUrl;
  removeUrlFragment(pURL->bstrVal, &bstrUrl);
  m_Frames[(BSTR) bstrUrl] = pWebBrowser;

  // Store the URL being loaded (used by the protocol sink).
  HRESULT hr = m_pWebBrowser->PutProperty(L"_anchoLoadingURL", *pURL);
  ATLASSERT(SUCCEEDED(hr));

  // Check if this is a new tab we are creating programmatically.
  // If so redirect it to the correct URL.
  std::wstring url(pURL->bstrVal, SysStringLen(pURL->bstrVal));
  //TODO - use headers instead of url
  size_t first = url.find_first_of(L'#');
  size_t last = url.find_last_of(L'#');
  if (first == std::wstring::npos || first == last) {
    return;
  }

  std::wstring requestIDStr = url.substr(first+1, last - first - 1);
  int requestID = _wtoi(requestIDStr.c_str());
  url.erase(0, last+1);

  CComVariant urlVar(url.c_str());
  CComVariant vtEmpty;

  *Cancel = TRUE;
  pWebBrowser->Stop();
  pWebBrowser->Navigate2(&urlVar, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);

  m_pAnchoService->createTabNotification(m_TabID, requestID);
}

STDMETHODIMP_(void) CAnchoRuntime::OnNewWindow3(IDispatch *pDisp, VARIANT_BOOL Cancel, DWORD dwFlags,	BSTR bstrUrlContext, BSTR bstrUrl)
{
  ATLTRACE(L"OnNewWindow3-------------------\n");
}

//----------------------------------------------------------------------------
//  OnFrameStart
STDMETHODIMP CAnchoRuntime::OnFrameStart(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame)
{
  return InitializeContentScripting(bstrUrl, bIsMainFrame, documentLoadStart);
}

//----------------------------------------------------------------------------
//  OnFrameEnd
STDMETHODIMP CAnchoRuntime::OnFrameEnd(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame)
{
  return InitializeContentScripting(bstrUrl, bIsMainFrame, documentLoadEnd);
}

//----------------------------------------------------------------------------
//  OnFrameRedirect
STDMETHODIMP CAnchoRuntime::OnFrameRedirect(BSTR bstrOldUrl, BSTR bstrNewUrl)
{
  CComBSTR url;
  removeUrlFragment(bstrOldUrl, &url);
  FrameMap::iterator it = m_Frames.find(bstrOldUrl);
  if (it != m_Frames.end()) {
    removeUrlFragment(bstrNewUrl, &url);
    m_Frames[(BSTR) url] = it->second;
    m_Frames.erase(it);
  }
  return S_OK;
}

//----------------------------------------------------------------------------
//  InitializeContentScripting
HRESULT CAnchoRuntime::InitializeContentScripting(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame, documentLoadPhase aPhase)
{
  CComPtr<IWebBrowser2> webBrowser;
  if (bIsMainFrame) {
    webBrowser = m_pWebBrowser;
  }
  else {
    CComBSTR url;
    removeUrlFragment(bstrUrl, &url);
    FrameMap::iterator it = m_Frames.find((BSTR) url);
    if (it == m_Frames.end()) {
      ATLASSERT(documentLoadEnd == aPhase);
      // Assume we have removed this frame while waiting for the document to complete.
      return S_FALSE;
    }
    webBrowser = it->second;
  }
  // We have to check whether we are the main web browser since the event source can't
  // always tell us if we are the main frame.
  if ((bIsMainFrame || webBrowser == m_pWebBrowser) && (documentLoadStart == aPhase)) {
    m_Frames.clear();
  }
  AddonMap::iterator it = m_Addons.begin();
  while(it != m_Addons.end()) {
    it->second->InitializeContentScripting(webBrowser, bstrUrl, aPhase);
    ++it;
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
    hr = Init();
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

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoRuntime::reloadTab()
{
  CComVariant var(REFRESH_COMPLETELY);
  m_pWebBrowser->Refresh2(&var);
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoRuntime::closeTab()
{
  return m_pWebBrowser->Quit();
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoRuntime::executeScript(BSTR aExtensionId, BSTR aCode, INT aFileSpecified)
{
  //TODO: check permissions from manifest
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoRuntime::updateTab(LPDISPATCH aProperties)
{
  CIDispatchHelper properties(aProperties);
  CComBSTR url;
  HRESULT hr = properties.Get<CComBSTR, VT_BSTR, BSTR>(L"url", url);
  if (hr == S_OK) {
    CComVariant vtUrl(url);
    CComVariant vtEmpty;
    m_pWebBrowser->Navigate2(&vtUrl, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
  }
  INT active = 0;
  hr = properties.Get<INT, VT_BOOL, INT>(L"active", active);
  if (hr == S_OK) {
    HWND hwnd = getTabWindow();
    IAccessible *acc = NULL;
    //TODO - fix tab activation
    if (S_OK == AccessibleObjectFromWindow(hwnd, OBJID_WINDOW, IID_IAccessible, (void**)&acc)) {
      CComVariant var(CHILDID_SELF, VT_I4);
      acc->accDoDefaultAction(var);
    }
  }
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoRuntime::fillTabInfo(VARIANT* aInfo)
{
  ENSURE_RETVAL(aInfo);
  if(aInfo->vt != VT_DISPATCH) {
    return E_NOINTERFACE;
  }
  CIDispatchHelper obj(aInfo->pdispVal);

  CComBSTR locationUrl;
  CComBSTR name;
  m_pWebBrowser->get_LocationURL(&locationUrl);
  obj.SetProperty(L"url", CComVariant(locationUrl));

  m_pWebBrowser->get_Name(&name);
  obj.SetProperty(L"title", CComVariant(name));

  obj.SetProperty(L"id", CComVariant(m_TabID));

  obj.SetProperty(L"active", CComVariant(isTabActive()));

  obj.SetProperty(L"windowId", reinterpret_cast<INT>(getMainWindow()));
  return S_OK;
}

//----------------------------------------------------------------------------
//
HWND CAnchoRuntime::getTabWindow()
{
  HWND hwndBrowser = NULL;
  IServiceProvider* pServiceProvider = NULL;
  if (SUCCEEDED(m_pWebBrowser->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider))){
    IOleWindow* pWindow = NULL;
    if (SUCCEEDED(pServiceProvider->QueryService(SID_SShellBrowser, IID_IOleWindow,(void**)&pWindow))) {
      // hwndBrowser is the handle of TabWindowClass
      if (!SUCCEEDED(pWindow->GetWindow(&hwndBrowser))) {
        hwndBrowser = NULL;
      }
      pWindow->Release();
    }
    pServiceProvider->Release();
  }
  return hwndBrowser;
}

//----------------------------------------------------------------------------
//
HWND CAnchoRuntime::findParentWindowByClass(std::wstring aClassName)
{
  HWND window = getTabWindow();
  wchar_t className[256];
  while (window) {
    if (!GetClassName(window, className, 256)) {
      return NULL;
    }
    if (aClassName == className) {
      return window;
    }
    window = GetParent(window);
  }
  return NULL;
}
//----------------------------------------------------------------------------
//
bool CAnchoRuntime::isTabActive()
{
  HWND hwndBrowser = getTabWindow();
  return hwndBrowser && ::IsWindowVisible(hwndBrowser);
}
