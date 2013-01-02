#include "stdafx.h"
#include "PopupWindow.h"
#include "AnchoBgSrv_i.h"

HRESULT CPopupWindow::FinalConstruct()
{
  m_WebBrowserEventsCookie = 0;
  return S_OK;
}

void CPopupWindow::FinalRelease()
{
  int asd = 0;
}

void CPopupWindow::OnFinalMessage(HWND)
{
  // This Release call is paired with the AddRef call in OnCreate.
  Release();
}

LRESULT CPopupWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  DefWindowProc();

  CComPtr<IAxWinHostWindow> spHost;
  IF_FAILED_RET2(QueryHost(__uuidof(IAxWinHostWindow), (void**)&spHost), -1);

  CComPtr<IUnknown>  p;
  IF_FAILED_RET2(spHost->CreateControlEx(m_sURL, *this, NULL, &p, /*DIID_DWebBrowserEvents2, GetEventUnk()*/ IID_NULL, NULL), -1);

  m_pWebBrowser = p;
  if (!m_pWebBrowser)
  {
    return -1;
  }

  AtlAdvise(m_pWebBrowser, (IUnknown *)(PopupWebBrowserEvents *) this, DIID_DWebBrowserEvents2, &m_WebBrowserEventsCookie);


  CIDispatchHelper script = CIDispatchHelper::GetScriptDispatch(m_pWebBrowser);
  for (DispatchMap::iterator it = m_InjectedObjects.begin(); it != m_InjectedObjects.end(); ++it) {
    ATLTRACE(L"INJECTING OBJECT %s\n", it->first.c_str());
    script.SetProperty((LPOLESTR)(it->first.c_str()), CComVariant(it->second));
  }

  //Replacing XMLHttpRequest by wrapper
  CComPtr<IDispatchEx> pRequest;
  IF_FAILED_RET(pRequest.CoCreateInstance(__uuidof(AnchoXmlHttpRequest)));

  CIDispatchHelper window;
  script.Get<CIDispatchHelper, VT_DISPATCH, IDispatch*>(L"window", window);
  if (window) {
    IF_FAILED_RET(window.SetProperty((LPOLESTR)L"XMLHttpRequest", CComVariant(pRequest.p)));
  }

  // This AddRef call is paired with the Release call in OnFinalMessage
  // to keep the object alive as long as the window exists.
  AddRef();
  return 0;
}

LRESULT CPopupWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
  AtlUnadvise(m_pWebBrowser, DIID_DWebBrowserEvents2, m_WebBrowserEventsCookie);
  bHandled = FALSE;
  //Cleanup procedure
  m_CloseCallback.Invoke0(DISPID(0));
  m_pWebBrowser.Release();
  return 1;
}

LRESULT CPopupWindow::OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  if (wParam == WA_INACTIVE) {
    DestroyWindow();
    return 0;
  }
  return 1;
}

STDMETHODIMP_(void) CPopupWindow::OnBrowserProgressChange(LONG Progress, LONG ProgressMax)
{
  //Workaround to rid of the ActiveXObject
  //?? still some scripts are started earlier ??
  //also executed multiple times
  CIDispatchHelper script = CIDispatchHelper::GetScriptDispatch(m_pWebBrowser);
  CIDispatchHelper window;
  script.Get<CIDispatchHelper, VT_DISPATCH, IDispatch*>(L"window", window);
  window.SetProperty((LPOLESTR)L"ActiveXObject", CComVariant());

  //Autoresize
  IDispatch *doc = NULL;
  if (FAILED(m_pWebBrowser->get_Document(&doc)) || !doc) {
    return;
  }
  CComQIPtr<IHTMLDocument2> htmlDocument2 = doc;
  if (!htmlDocument2) {
    return;
  }
  CComPtr<IHTMLElement> element;
  if (FAILED(htmlDocument2->get_body(&element)) || !element ) {
    return;
  }

  CComQIPtr<IHTMLElement2> element2 = element;
  long contentHeight, contentWidth;
  if (FAILED(element->get_offsetHeight(&contentHeight)) ||
    FAILED(element->get_offsetWidth(&contentWidth)))
  {
    return;
  }
  if (contentHeight > 0 && contentWidth > 0) {
    CRect rect;
    BOOL res = this->GetWindowRect(rect);

    if (res && (rect.Height() != contentHeight || rect.Width() != contentWidth)) {
      this->MoveWindow(rect.left, rect.top, contentWidth, contentHeight, TRUE);
    }
  }
}

HRESULT CPopupWindow::CreatePopupWindow(HWND aParent, const DispatchMap &aInjectedObjects, LPCWSTR aURL, int aX, int aY, CIDispatchHelper aCloseCallback)
{
  CPopupWindowComObject * pNewWindow = NULL;
  IF_FAILED_RET(CPopupWindowComObject::CreateInstance(&pNewWindow));
  pNewWindow->m_sURL = aURL;
  pNewWindow->m_InjectedObjects = aInjectedObjects;
  pNewWindow->m_CloseCallback = aCloseCallback;
  RECT r = {aX, aY, aX + 400, aY + 400};

  if (!pNewWindow->Create(aParent, r, NULL, WS_POPUP))
  {
    return E_FAIL;
  }
  pNewWindow->ShowWindow(SW_SHOW);
  return S_OK;
}