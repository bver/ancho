/****************************************************************************
 * AnchoAddonService.cpp : Implementation of CAnchoAddonService
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoAddonService.h"

#include <shlguid.h>
#include <OleAcc.h> // For ObjectFromLresult
#pragma comment(lib, "Oleacc.lib")
/*============================================================================
 * class CAnchoAddonBackground
 */

//----------------------------------------------------------------------------
//
void CAnchoAddonService::OnAddonFinalRelease(BSTR bsID)
{
  m_Objects.RemoveKey(bsID);
}

//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::invokeExternalEventObject(BSTR aExtensionId, BSTR aEventName, LPDISPATCH aArgs, VARIANT* aRet)
{
  CAnchoAddonBackgroundComObject* pObject = NULL;

  if (m_Objects.Lookup(aExtensionId, pObject))
  {
    return pObject->invokeExternalEventObject(aExtensionId, aEventName, aArgs, aRet);
  }
  return S_OK;
}

//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::openNewTab(LPUNKNOWN aWebBrowserWin, BSTR url) {
  //LPUNKNOWN lpWebBrowser = NULL;
  //m_ApiService->get_activeWebBrowser(&lpWebBrowser);
  CComQIPtr<IWebBrowser2> webBrowser = aWebBrowserWin;
  if (!webBrowser) {
    return E_POINTER;
  }
  
  CComVariant vtUrl(url);
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
HRESULT CAnchoAddonService::FinalConstruct()
{
  // Get and store the path, this will be used in some places (e.g. to load
  // additional DLLs).
  LPTSTR psc = m_sThisPath.GetBuffer(_MAX_PATH);
  GetModuleFileName(_AtlModule.m_hInstance, psc, _MAX_PATH);
  PathRemoveFileSpec(psc);
  PathAddBackslash(psc);
  m_sThisPath.ReleaseBuffer();
  return S_OK;
}

//----------------------------------------------------------------------------
//
void CAnchoAddonService::FinalRelease()
{
  CAnchoAddonBackgroundComObject* pObject;
  CString sID;
  POSITION pos = m_Objects.GetStartPosition();
  while(pos)
  {
    m_Objects.GetNextAssoc(pos, sID, pObject);
    pObject->AddonServiceLost();
  }
  m_Objects.RemoveAll();
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::GetExtension(BSTR bsID, IAnchoAddonBackground ** ppRet)
{
  ENSURE_RETVAL(ppRet);

  CAnchoAddonBackgroundComObject* pObject = NULL;
  CComPtr<IAnchoAddonBackground> ptr;

  if (!m_Objects.Lookup(bsID, pObject))
  {
    // not found, create new instance
    ATLTRACE(_T("ADD OBJECT %s\n"), bsID);
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
    hr = pObject->Init(m_sThisPath, this, bsID);
    if (FAILED(hr))
    {
      return hr;
    }
    // store in map
    m_Objects[bsID] = pObject;
  }
  else
  {
    ATLTRACE(_T("FOUND OBJECT %s\n"), bsID);
    // found, create a new intance ID
    // store to safepointer
    ptr = pObject;
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

STDMETHODIMP CAnchoAddonService::registerRuntime(IAnchoRuntime * aRuntime, INT aTabID)
{
  m_Runtimes[aTabID] = RuntimeRecord(aRuntime);
  return S_OK;
}

STDMETHODIMP CAnchoAddonService::unregisterRuntime(INT aTabID)
{
  m_Runtimes.erase(aTabID);
  return S_OK;
}


//----------------------------------------------------------------------------
//

BOOL CALLBACK EnumBrowserWindows(HWND hwnd, LPARAM lParam)
{
  wchar_t className[MAX_PATH];
  ::GetClassName(hwnd, className, MAX_PATH);
  if (wcscmp(className, L"Internet Explorer_Server") == 0) {
    // Now we need to get the IWebBrowser2 from the window.
    DWORD dwMsg = ::RegisterWindowMessage(L"WM_HTML_GETOBJECT");
    LRESULT lResult = 0;
    ::SendMessageTimeout(hwnd, dwMsg, 0, 0, SMTO_ABORTIFHUNG, 1000, (DWORD*) &lResult);
    if (lResult) {
      CComPtr<IHTMLDocument2> doc;
      HRESULT hr = ::ObjectFromLresult(lResult, IID_IHTMLDocument2, 0, (void**) &doc);
      if (SUCCEEDED(hr)) {
        CComPtr<IHTMLWindow2> win;
        hr = doc->get_parentWindow(&win);
        if (SUCCEEDED(hr)) {
          CComQIPtr<IServiceProvider> sp(win);
          CComPtr<IWebBrowser2> pWebBrowser;
          if (sp) {
            hr = sp->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**) &pWebBrowser);
            if (SUCCEEDED(hr)) {
              CComPtr<IDispatch> container;
              pWebBrowser->get_Container(&container);
              // IWebBrowser2 doesn't have a container if it is an IE tab, so if we have a container
              // then we must be an embedded web browser (e.g. in an HTML toolbar).
              if (!container) {
                // Now get the HWND associated with the tab so we can see if it is active.
                sp = pWebBrowser;
                if (sp) {
                  CComPtr<IOleWindow> oleWindow;
                  hr = sp->QueryService(SID_SShellBrowser, IID_IOleWindow, (void**) &oleWindow);
                  if (SUCCEEDED(hr)) {
                    HWND hTab;
                    hr = oleWindow->GetWindow(&hTab);
                    if (SUCCEEDED(hr) && ::IsWindowEnabled(hTab)) {
                      // Success, we found the active browser!
                      pWebBrowser.CopyTo((IWebBrowser2 **) lParam);
                      return FALSE;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return TRUE;
}

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