#include "stdafx.h"
#include "Toolbar.h"

void CToolbar::GetBandInfoValues(const wchar_t *& title, POINTL &minSize)
{
  title = L"TEST_TOOLBAR";
  minSize.x = 200;
  minSize.y = 28;
}

HRESULT CToolbar::InternalSetSite()
{
  IF_FAILED_RET(BaseClass::InternalSetSite());

  // create addon service object
  IF_FAILED_RET(mAnchoService.CoCreateInstance(CLSID_AnchoAddonService));
  CComBSTR url;
  mAnchoService->registerBrowserActionToolbar(&url);
  mUrl = std::wstring(url);

  CComPtr<IDispatch> dispatchObject;
  mAnchoService->getDispatchObject(&dispatchObject);
  mContentWindow->setExternalObject(dispatchObject);
  return RunToolbarPage();
}

HRESULT CToolbar::InternalReleaseSite()
{
  //mContentWindow->m_External.Release();
  return BaseClass::InternalReleaseSite();
}

STDMETHODIMP CToolbar::TranslateAcceleratorIO(LPMSG lpMsg)
{
  CComQIPtr<IOleInPlaceActiveObject, &IID_IOleInPlaceActiveObject> pIOIPAO(mContentWindow->m_pWebBrowser);
  if (!pIOIPAO || pIOIPAO->TranslateAccelerator(lpMsg) != S_OK) {
    TranslateMessage(lpMsg);
    DispatchMessage(lpMsg);
  }

  return S_OK;
}

STDMETHODIMP CToolbar::HasFocusIO(void)
{
  HWND hFocusWnd = ::GetFocus();
  return ::IsChild((HWND)*mContentWindow, hFocusWnd) ? S_OK : S_FALSE;
}

STDMETHODIMP CToolbar::UIActivateIO(BOOL fActivate, LPMSG lpMsg)
{
  if (fActivate) {
    mContentWindow->SetFocus();
  }
  return S_OK;
}

HRESULT CToolbar::RunToolbarPage()
{
  mContentWindow->Create(mHWNDParent, CWindow::rcDefault, L"about:blank",
      WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);

  if (!mContentWindow->m_pWebBrowser)
  {
    return E_FAIL;
  }
  mHWNDParent = mContentWindow->operator HWND();

  return mContentWindow->m_pWebBrowser->Navigate(CComBSTR(mUrl.c_str()), NULL, NULL, NULL, NULL);
}

void CToolbar::ToolbarWindowBeforeNavigate()
{
  //DestroyMagpieInstance(m_Magpie);
}

void CToolbar::ToolbarWindowReady(VARIANT *pURL)
{
  CString url(pURL->bstrVal);
  if (url != _T("about:blank"))
  {
    /*HRESULT hr = ExecuteContentScript();
    ATLASSERT(SUCCEEDED(hr));*/
  }
}

void CToolbar::ToolbarWindowNavigateComplete(IDispatch *pDisp, VARIANT *URL)
{

}

void CToolbar::ToolbarWindowOnSetFocus()
{
  HRESULT hr;
  if (mInputObjectSite) {
    CComPtr<IUnknown> pUnk;
    if (SUCCEEDED(QueryInterface(IID_IUnknown, (LPVOID*) &pUnk.p)))
    {
      hr = mInputObjectSite->OnFocusChangeIS(pUnk, TRUE);
      ATLASSERT(SUCCEEDED(hr));
    }
  }
}

void CToolbar::ToolbarWindowOnKillFocus()
{
  HRESULT hr;
  // If an input field in the toolbar has the focus when the user clicks on the main
  // browser window, the caret stays in the input field instead of disappearing. Sending
  // WM_UPDATEUISTATUS manually to this window doesn't seem to help, so we simply
  // change the active element to make sure that the caret disappears.
  CComQIPtr<IDispatch> documentDispatch;
  if (mContentWindow && SUCCEEDED(mContentWindow->m_pWebBrowser->get_Document(&documentDispatch))) {
    CComQIPtr<IHTMLDocument3> document(documentDispatch);
    if (document) {
      CComQIPtr<IHTMLElement> element;
      if (SUCCEEDED(document->get_documentElement(&element))) {
        CComQIPtr<IHTMLElement3> element3(element);
        if (element3) {
          element3->setActive();
        }
      }
    }
  }

  if (mInputObjectSite) {
    CComPtr<IUnknown> pUnk;
    if (SUCCEEDED(QueryInterface(IID_IUnknown, (LPVOID*) &pUnk.p)))
    {
      hr = mInputObjectSite->OnFocusChangeIS(pUnk, FALSE);
      ATLASSERT(SUCCEEDED(hr));
    }
  }
}
