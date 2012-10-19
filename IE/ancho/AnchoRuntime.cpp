/****************************************************************************
 * AnchoRuntime.cpp : Implementation of CAnchoRuntime
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoRuntime.h"
#include "AnchoAddon.h"
#include "dllmain.h"

#include <string>

#include <Iepmapi.h>
#pragma comment(lib, "Iepmapi.lib")

extern class CanchoModule _AtlModule;

/*============================================================================
 * class CAnchoRuntime
 */

//----------------------------------------------------------------------------
//  InitAddons
HRESULT CAnchoRuntime::InitAddons()
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
  AtlAdvise(m_pWebBrowser, (IUnknown*)(DWebBrowserEvents2AnchoRuntime*)this, DIID_DWebBrowserEvents2, &m_dwAdviseSinkWebBrowser);

  // create addon service object
  IF_FAILED_RET(m_pAnchoService.CoCreateInstance(CLSID_AnchoAddonService));

  //Registering tab in service - obtains tab id and assigns it to the tab as property
  IF_FAILED_RET(m_pAnchoService->registerRuntime(this, &m_TabID));
  HWND hwnd;  m_pWebBrowser->get_HWND((long*)&hwnd);
  ::SetProp(hwnd, s_AnchoTabIDPropertyName, (HANDLE)m_TabID);

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
    if (m_dwAdviseSinkWebBrowser)
    {
      AtlUnadvise(m_pWebBrowser, DIID_DWebBrowserEvents2, m_dwAdviseSinkWebBrowser);
      m_dwAdviseSinkWebBrowser = 0;
    }
    m_pWebBrowser.Release();
  }
}

//----------------------------------------------------------------------------
//  SetSite
STDMETHODIMP CAnchoRuntime::SetSite(IUnknown *pUnkSite)
{
  HRESULT hr = IObjectWithSiteImpl<CAnchoRuntime>::SetSite(pUnkSite);
  IF_FAILED_RET(hr);
  if (pUnkSite)
  {
    hr = InitAddons();
  }
  else
  {
    DestroyAddons();
  }
  return hr;
}
//----------------------------------------------------------------------------
//
STDMETHODIMP_(void) CAnchoRuntime::browserBeforeNavigateEvent(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel)
{
  CComQIPtr<IWebBrowser2> pWebBrowser(pDisp);
  if (!pWebBrowser) {
    return;
  }
  if(pURL->vt != VT_BSTR) {
    return;
  }
  std::wstring url(pURL->bstrVal, SysStringLen(pURL->bstrVal));

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
    ::EnableWindow(hwnd, TRUE);
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
bool CAnchoRuntime::isTabActive()
{
  HWND hwndBrowser = getTabWindow();
  return hwndBrowser && ::IsWindowEnabled(hwndBrowser);
}