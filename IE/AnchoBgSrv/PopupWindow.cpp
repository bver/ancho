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
  IF_FAILED_RET(script.SetProperty((LPOLESTR)L"XMLHttpRequest", CComVariant(pRequest.p)));

  CIDispatchHelper window;
  script.Get<CIDispatchHelper, VT_DISPATCH, IDispatch*>(L"window", window);
  if (window) {
    IF_FAILED_RET(window.SetProperty((LPOLESTR)L"XMLHttpRequest", CComVariant(pRequest.p)));
    /*IF_FAILED_RET(window.SetProperty((LPOLESTR)L"ActiveXObject", CComVariant(pRequest.p)));
    CIDispatchHelper activeXObject;
    window.Get<CIDispatchHelper, VT_DISPATCH, IDispatch*>(L"ActiveXObject", activeXObject);*/
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

//STDMETHODIMP_(void) CPopupWindow::OnDownloadComplete()
STDMETHODIMP_(void) CPopupWindow::OnNavigateComplete(LPDISPATCH aDispatch, VARIANT *aURL)
{
  //ATLTRACE(L"NAVIGATE COMPLETE: %s\n", aURL->bstrVal);
  //CComQIPtr<IWebBrowser2> webBrowser(aDispatch);

  //if (webBrowser != m_pWebBrowser) {
  //  return;
  //}

  IDispatch *doc = NULL;
  m_pWebBrowser->get_Document(&doc);

  CComQIPtr<IHTMLDocument2> htmlDocument2 = doc;
  CComQIPtr<IHTMLDocument3> htmlDocument3 = doc;
  if (!htmlDocument2 || !htmlDocument3) {
    return;
  }

  CComBSTR code(L"alert('A');console.log('This is injected code');");

  //CComBSTR tagName(L"DIV");
  CComBSTR tagName(L"SCRIPT");
  CComBSTR scriptType(L"text/javascript");
  CComPtr<IHTMLElement> scriptTag;
  htmlDocument2->createElement(tagName, &scriptTag);
  scriptTag->put_innerText(code);

  CComPtr<IHTMLElementCollection> headCollection;
  //htmlDocument3->getElementsByTagName(CComBSTR(L"BODY"), &headCollection);
  htmlDocument3->getElementsByTagName(CComBSTR(L"HEAD"), &headCollection);

  CComPtr<IDispatch> headItem;
  headCollection->item(CComVariant((int)0), CComVariant((int)0), &headItem);
  CComQIPtr<IHTMLDOMNode> headNode = headItem;

  CComPtr<IHTMLDOMNode> retNode;
  CComQIPtr<IHTMLDOMNode> insertedNode = scriptTag;
  headNode->appendChild(insertedNode, &retNode);
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