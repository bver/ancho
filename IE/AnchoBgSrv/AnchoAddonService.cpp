/****************************************************************************
 * AnchoAddonService.cpp : Implementation of CAnchoAddonService
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoAddonService.h"

#include <sstream>
#include <algorithm>

struct CookieNotificationCallback: public ACookieCallbackFunctor
{
  CookieNotificationCallback(CAnchoAddonService &aService): service(aService)
  {}

  void operator()(CComVariant &aCookie)
  {
    ATLASSERT(aCookie.vt == VT_DISPATCH);
    CComBSTR eventName(L"cookies.onChanged");
    
    service.invokeEventObjectInAllExtensionsWithIDispatchArgument(eventName, aCookie.pdispVal);
    ATLTRACE("NOTIFICATION ");
  }

  CAnchoAddonService &service;
};


/*============================================================================
 * class CAnchoAddonService
 */

//----------------------------------------------------------------------------
//
void CAnchoAddonService::OnAddonFinalRelease(BSTR bsID)
{
  m_BackgroundObjects.erase(std::wstring(bsID, SysStringLen(bsID)));
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::get_cookieManager(LPDISPATCH* ppRet)
{
  ENSURE_RETVAL(ppRet);
  return m_Cookies.QueryInterface(ppRet);
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::invokeExternalEventObject(BSTR aExtensionId, BSTR aEventName, LPDISPATCH aArgs, VARIANT* aRet)
{
  CAnchoAddonBackgroundComObject* pObject = NULL;

  BackgroundObjectsMap::iterator it = m_BackgroundObjects.find(std::wstring(aExtensionId, SysStringLen(aExtensionId)));
  if (it != m_BackgroundObjects.end()) {
    ATLASSERT(it->second != NULL);
    return it->second->invokeExternalEventObject(aEventName, aArgs, aRet);
  }
  return S_OK;
}

//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::navigateBrowser(LPUNKNOWN aWebBrowserWin, const std::wstring &url)
{
  CComQIPtr<IWebBrowser2> webBrowser = aWebBrowserWin;
  if (!webBrowser) {
    return E_NOINTERFACE;
  }

  CComVariant vtUrl(url.c_str());
  CComVariant vtFlags(navOpenInNewTab);
  CComVariant vtEmpty;
  return webBrowser->Navigate2(&vtUrl, &vtFlags, &vtEmpty, &vtEmpty, &vtEmpty);
}

//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::getActiveWebBrowser(LPUNKNOWN* pUnkWebBrowser)
{
  CComPtr<IWebBrowser2> pWebBrowser;
  HRESULT hr = FindActiveBrowser(&pWebBrowser);
  if (FAILED(hr)) {
    return hr;
  }
  return pWebBrowser->QueryInterface(IID_IUnknown, (void**) pUnkWebBrowser);
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::createTab(LPDISPATCH aProperties, LPDISPATCH aCreator, LPDISPATCH aCallback)
{
  try {
    m_WebBrowserPostInitTasks.addCommnad(ACommand::Ptr(new CreateTabCommand(*this, aProperties, aCreator, aCallback)));
  } catch (std::runtime_error &e) {
    ATLTRACE("Error: %s\n", e.what());
    return E_FAIL;
  }
  return S_OK;
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::createTabImpl(CIDispatchHelper &aProperties, CIDispatchHelper &aCreator, CIDispatchHelper &aCallback)
{
  //CIDispatchHelper properties(aProperties);
  CComBSTR originalUrl;
  HRESULT hr = aProperties.Get<CComBSTR, VT_BSTR, BSTR>(L"url", originalUrl);
  if (hr != S_OK) {
    return hr;
  }
  std::wstring url = std::wstring(originalUrl,SysStringLen(originalUrl));
  if (aCallback) {
    //TODO - use headers instead of url
    int requestID = m_NextRequestID++;
    std::wostringstream str;
    str << L'#' << requestID << '#';
    url = str.str() + url;

    m_CreateTabCallbacks[requestID] = CreateTabCallbackRecord(aCreator, aCallback);
  }

  LPUNKNOWN browser;
  IF_FAILED_RET(getActiveWebBrowser(&browser));
  IF_FAILED_RET(navigateBrowser(browser, url));
  return S_OK;
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::reloadTab(INT aTabId)
{
  RuntimeMap::iterator it = m_Runtimes.find(aTabId);
  if (it != m_Runtimes.end()) {
    ATLASSERT(it->second.runtime);
    it->second.runtime->reloadTab();
    return S_OK;
  }
  return E_FAIL;
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::removeTabs(LPDISPATCH aTabs, LPDISPATCH aCallback)
{
  VariantVector tabs;

  IF_FAILED_RET(addJSArrayToVariantVector(aTabs, tabs));
  for(VariantVector::iterator it = tabs.begin(); it != tabs.end(); ++it) {
    if( it->vt == VT_I4 ) {
      removeTab(it->intVal, aCallback);
    } else {
      ATLTRACE(L"Problem with specified tabId - not an integer\n");
    }
  }
  return S_OK;
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::removeTab(INT aTabId, LPDISPATCH aCallback)
{
  RuntimeMap::iterator it = m_Runtimes.find(aTabId);
  if (it != m_Runtimes.end()) {
    ATLASSERT(it->second.runtime);
    if (aCallback) {
      it->second.callback = aCallback;
    }
    it->second.runtime->closeTab();
    return S_OK;
  } else {
    //If we don't find the tab - call the callback,
    //so we don't lose track of tabs for removal.
    CIDispatchHelper callback = aCallback;
    CComVariant tabId(aTabId);
    callback.Invoke1((DISPID)0, &tabId);
  }
  return E_FAIL;
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::updateTab(INT aTabId, LPDISPATCH aProperties)
{
  RuntimeMap::iterator it = m_Runtimes.find(aTabId);
  if (it != m_Runtimes.end()) {
    ATLASSERT(it->second.runtime);
    it->second.runtime->updateTab(aProperties);
    return S_OK;
  }
  return E_FAIL;
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::getTabInfo(INT aTabId, LPDISPATCH aCreator, VARIANT* aRet)
{
  HRESULT hr = createIDispatchFromCreator(aCreator, aRet);
  if (hr != S_OK) {
    return hr;
  }

  RuntimeMap::iterator it = m_Runtimes.find(aTabId);
  if (it != m_Runtimes.end()) {
    ATLASSERT(it->second.runtime);
    return it->second.runtime->fillTabInfo(aRet);
  }
  aRet->vt = VT_EMPTY;
  return S_OK;
}

//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::queryTabs(LPDISPATCH aQueryInfo, LPDISPATCH aCreator, VARIANT* aRet)
{
  ENSURE_RETVAL(aRet);
  struct QueryTabFunctor
  {
    QueryTabFunctor(VariantVector &aInfos, LPDISPATCH aCreator, CAnchoAddonService &aService ): infos(aInfos), creator(aCreator), service(aService) {}
    void operator()(RuntimeMap::value_type &aRec)
    {
      CComVariant info;
      /*if (S_OK != createIDispatchFromCreator(creator, &info)) {
        return;
      }*/
      if (S_OK == service.getTabInfo(aRec.first, creator, &info)) {
        infos.push_back(info);
      }
    }
    VariantVector &infos;
    LPDISPATCH creator;
    CAnchoAddonService &service;
  };


  VariantVector infos;
  std::for_each(m_Runtimes.begin(), m_Runtimes.end(), QueryTabFunctor(infos, aCreator, *this));
  return constructSafeArrayFromVector(infos, *aRet);
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::executeScript(BSTR aExtensionID, INT aTabID, BSTR aCode, BOOL aFileSpecified, BOOL aInAllFrames)
{
  if (aInAllFrames) {
    //TODO: gather results
    for (RuntimeMap::iterator it = m_Runtimes.begin(); it != m_Runtimes.end(); ++it) {
      executeScriptInTab(aExtensionID, it->first, aCode, aFileSpecified);
    }
  } else {
    return executeScriptInTab(aExtensionID, aTabID, aCode, aFileSpecified);
  }
  return S_OK;
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::executeScriptInTab(BSTR aExtensionID, INT aTabID, BSTR aCode, BOOL aFileSpecified)
{
  RuntimeMap::iterator it = m_Runtimes.find(aTabID);
  if (it != m_Runtimes.end()) {
    it->second.runtime->executeScript(aExtensionID, aCode, aFileSpecified);
  }
  return S_OK;
}
//----------------------------------------------------------------------------
//

static void fillWindowInfo(HWND aWndHandle, CIDispatchHelper &aInfo)
{
  //BOOL isVisible = IsWindowVisible(aWndHandle);
  WINDOWINFO winInfo;
  winInfo.cbSize = sizeof(WINDOWINFO);
  BOOL res = GetWindowInfo(aWndHandle, &winInfo);
  aInfo.SetProperty(L"top", CComVariant(winInfo.rcWindow.top));
  aInfo.SetProperty(L"left", CComVariant(winInfo.rcWindow.left));
  aInfo.SetProperty(L"width", CComVariant(winInfo.rcWindow.right - winInfo.rcWindow.left));
  aInfo.SetProperty(L"height", CComVariant(winInfo.rcWindow.bottom - winInfo.rcWindow.top));
  aInfo.SetProperty(L"focused", CComVariant(static_cast<bool>(winInfo.dwWindowStatus & WS_ACTIVECAPTION)));
  aInfo.SetProperty(L"alwaysOnTop", CComVariant(false));
  aInfo.SetProperty(L"id", CComVariant(reinterpret_cast<INT>(aWndHandle)));
  if (IsIconic(aWndHandle)) {
    aInfo.SetProperty(L"state", CComVariant(L"minimized"));
  } else if (IsZoomed(aWndHandle)) {
    aInfo.SetProperty(L"state", CComVariant(L"maximized"));
  } else {
    aInfo.SetProperty(L"state", CComVariant(L"normal"));
  }
}

STDMETHODIMP CAnchoAddonService::getAllWindows(LPDISPATCH aCreator, VARIANT* aRet)
{
  if (!aCreator) {
    return E_POINTER;
  }
  ENSURE_RETVAL(aRet);

  VariantVector windowInfos;
  HWND hIEFrame = NULL;
  do {
    hIEFrame = ::FindWindowEx(NULL, hIEFrame, L"IEFrame", NULL);
    if (hIEFrame) {
      CComVariant info;
      if (S_OK != createIDispatchFromCreator(aCreator, &info)) {
        return E_FAIL;
      }
      CIDispatchHelper infoHelper(info.pdispVal);
      fillWindowInfo(hIEFrame, infoHelper);

      windowInfos.push_back(info);
    }
  }while(hIEFrame);
  return constructSafeArrayFromVector(windowInfos, *aRet);
}
//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::createWindow(LPDISPATCH aProperties, LPDISPATCH aCreator, LPDISPATCH aCallback)
{
  CIDispatchHelper properties(aProperties);

  std::wstring type;
  HRESULT hr = properties.Get<std::wstring, VT_BSTR, BSTR>(L"type", type);
  if (S_OK != hr || type == L"normal") {
    ATLTRACE(L"Creating normal browser window\n");
  } else if (type == L"popup") {
    ATLTRACE(L"Creating popup window\n");
  }

  std::wstring url = L"about:blank";
  hr = properties.Get<std::wstring, VT_BSTR, BSTR>(L"url", url);
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::getCurrentWindowId(INT *aWinId)
{
  ENSURE_RETVAL(aWinId);
  HRESULT ret = S_OK;
  HWND hIEFrame = NULL;
  do {
    hIEFrame = ::FindWindowEx(NULL, hIEFrame, L"IEFrame", NULL);
    if (hIEFrame) {
      WINDOWINFO winInfo;
      winInfo.cbSize = sizeof(WINDOWINFO);
      BOOL res = GetWindowInfo(hIEFrame, &winInfo);
      if (!res) {
        ret = E_FAIL;
        break;
      }
      if (winInfo.dwWindowStatus & WS_ACTIVECAPTION) {
        *aWinId = reinterpret_cast<INT>(hIEFrame);
        break;
      }
    }
  }while(hIEFrame);
  return ret;
}
//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::FinalConstruct()
{
  // Get and store the path, this will be used in some places (e.g. to load
  // additional DLLs).
  LPTSTR psc = m_sThisPath.GetBuffer(_MAX_PATH);
  GetModuleFileName(_AtlModule.m_hInstance, psc, _MAX_PATH);
  PathRemoveFileSpec(psc);
  PathAddBackslash(psc);
  m_sThisPath.ReleaseBuffer();

  CComObject<CIECookieManager> * pCookiesManager = NULL;
  IF_FAILED_RET(CComObject<CIECookieManager>::CreateInstance(&pCookiesManager));
  pCookiesManager->setNotificationCallback(ACookieCallbackFunctor::APtr(new CookieNotificationCallback(*this)));
  pCookiesManager->startWatching();
  
  m_Cookies = pCookiesManager;
  return S_OK;
}

//----------------------------------------------------------------------------
//
void CAnchoAddonService::FinalRelease()
{
  BackgroundObjectsMap::iterator it = m_BackgroundObjects.begin();
  while(it != m_BackgroundObjects.end())
  {
    ATLASSERT(it->second != NULL);
    it->second->OnAddonServiceReleased();
    ++it;
  }
  m_BackgroundObjects.clear();
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::GetAddonBackground(BSTR bsID, IAnchoAddonBackground ** ppRet)
{
  ENSURE_RETVAL(ppRet);

  CComPtr<IAnchoAddonBackground> ptr;
  std::wstring id = std::wstring(bsID, SysStringLen(bsID));
  BackgroundObjectsMap::iterator it = m_BackgroundObjects.find(id);
  if (it == m_BackgroundObjects.end()) {
    // not found, create new instance
    ATLTRACE(_T("ADD OBJECT %s\n"), bsID);
    CAnchoAddonBackgroundComObject* pObject = NULL;
    HRESULT hr = CAnchoAddonBackgroundComObject::CreateInstance(&pObject);
    if (FAILED(hr))
    {
      return hr;
    }
    // store to a safepointer
    ptr = pObject;

    ///////////////////////////////////////////////////////
    // Init the whole addon. This will load and init the
    // Ancho api.
    hr = pObject->Init(m_sThisPath, this, static_cast<IAnchoServiceApi*>(this), bsID);
    if (FAILED(hr))
    {
      return hr;
    }
    // store in map
    m_BackgroundObjects[id] = pObject;
  } else {
    ATLTRACE(_T("FOUND OBJECT %s\n"), bsID);
    // found, create a new intance ID
    // store to safepointer
    ptr = it->second;
  }

  // set return value
  (*ppRet) = ptr.Detach();
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::GetModulePath(BSTR * pbsPath)
{
  ENSURE_RETVAL(pbsPath);
  (*pbsPath) = m_sThisPath.AllocSysString();
  return S_OK;
}
//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::registerRuntime(IAnchoRuntime * aRuntime, INT *aTabID)
{
  ENSURE_RETVAL(aTabID);
  //Assigning tab id
  *aTabID = m_NextTabID++;

  m_Runtimes[*aTabID] = RuntimeRecord(aRuntime);
  ATLTRACE(L"ADDON SERVICE - registering tab: %d\n", *aTabID);
  return S_OK;
}
//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::unregisterRuntime(INT aTabID)
{
  RuntimeMap::iterator it = m_Runtimes.find(aTabID);
  if (it != m_Runtimes.end()) {
    CIDispatchHelper callback = it->second.callback;
    m_Runtimes.erase(it);
    if (callback) {
      ATLTRACE(L"ADDON SERVICE - invoking callback for removeTab: %d\n", aTabID);
      CComVariant tabId(aTabID);
      callback.Invoke1((DISPID)0, &tabId);
    }
  }
  ATLTRACE(L"ADDON SERVICE - unregistering tab: %d\n", aTabID);
  return S_OK;
}
//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::createTabNotification(INT aTabID, INT aRequestID)
{
  CreateTabCallbackMap::iterator it = m_CreateTabCallbacks.find(aRequestID);
  if (it != m_CreateTabCallbacks.end()) {
    CComVariant tabInfo;
    IF_FAILED_RET(getTabInfo(aTabID, it->second.creator, &tabInfo));
    IF_FAILED_RET(it->second.callback.Invoke1((DISPID) 0, &tabInfo));
    m_CreateTabCallbacks.erase(it);
    return S_OK;
  }
  return S_OK;
}
//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::invokeEventObjectInAllExtensions(BSTR aEventName, LPDISPATCH aArgs)
{
  for (BackgroundObjectsMap::iterator it = m_BackgroundObjects.begin(); it != m_BackgroundObjects.end(); ++it) {
    CComVariant retVal;
    CComBSTR id(it->first.c_str());
    invokeExternalEventObject(id, aEventName, aArgs, &retVal);
  }
  return S_OK;
}
//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::invokeEventObjectInAllExtensionsWithIDispatchArgument(BSTR aEventName, LPDISPATCH aArg)
{
  for (BackgroundObjectsMap::iterator it = m_BackgroundObjects.begin(); it != m_BackgroundObjects.end(); ++it) {
    it->second->invokeEventWithIDispatchArgument(aEventName, aArg);
  }
  return S_OK;
}
//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::webBrowserReady()
{
  m_WebBrowserPostInitTasks.setAutoExec(true);
  return S_OK;
}
//----------------------------------------------------------------------------
//

HRESULT CAnchoAddonService::FindActiveBrowser(IWebBrowser2** webBrowser)
{
  ATLASSERT(webBrowser != NULL);
  *webBrowser = NULL;
  // First find the IE frame windows.
  HWND hIEFrame = NULL;
  do {
    hIEFrame = ::FindWindowEx(NULL, hIEFrame, L"IEFrame", NULL);
    if (hIEFrame) {
      BOOL enable = ::IsWindowEnabled(hIEFrame);
      // Now we enumerate the child windows to find the "Internet Explorer_Server".
      ::EnumChildWindows(hIEFrame, EnumBrowserWindows, (LPARAM) webBrowser);
      if (*webBrowser) {
        return S_OK;
      }
    }
  }while(hIEFrame);

  // Oops, for some reason we didn't find it.
  return E_FAIL;
}
