#include "stdafx.h"
#include "PopupWindow.h"


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
    script.SetProperty((LPOLESTR)(it->first.c_str()), CComVariant(it->second));
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
  m_pWebBrowser.Release();
//  m_pDispApiJS.Release();
  return 1;
}

LRESULT CPopupWindow::OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
/*  if (wParam == WA_INACTIVE) {
    DestroyWindow();
    return 0;
  }*/
  return 1;
}

STDMETHODIMP_(void) CPopupWindow::OnDocumentComplete(LPDISPATCH aDispatch, VARIANT *aURL)
{
  ATLTRACE(L"DOCUMENT COMPLETE: %s\n", aURL->bstrVal);
  CComQIPtr<IWebBrowser2> webBrowser(aDispatch);
  ATLASSERT(webBrowser);

  if (webBrowser != m_pWebBrowser) {
    return;
  }

  IDispatch *doc = NULL;
  webBrowser->get_Document(&doc);

  CComQIPtr<IHTMLDocument2> htmlDocument = doc;
  if (!htmlDocument) {
    return;
  }
  ATLTRACE(L"DOCUMENT QUERY SUCCESS\n");
  //TODO - autoresize
}

HRESULT CPopupWindow::CreatePopupWindow(HWND aParent, const DispatchMap &aInjectedObjects, LPCWSTR aURL)
{
  CPopupWindowComObject * pNewWindow = NULL;
  IF_FAILED_RET(CPopupWindowComObject::CreateInstance(&pNewWindow));
  pNewWindow->m_sURL = aURL;
  pNewWindow->m_InjectedObjects = aInjectedObjects;
  RECT r = {50,50,550,550};

  if (!pNewWindow->Create(aParent, r, NULL, WS_POPUP))
  {
    return E_FAIL;
  }
  pNewWindow->ShowWindow(SW_SHOW);
  return S_OK;
}