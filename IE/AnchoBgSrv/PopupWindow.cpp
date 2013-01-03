#include "stdafx.h"
#include "PopupWindow.h"
#include "AnchoBgSrv_i.h"

class CPopupResizeEventHandler;
typedef CComObject<CPopupResizeEventHandler> CPopupResizeEventHandlerComObject;

class ATL_NO_VTABLE CPopupResizeEventHandler :
    public CComObjectRootEx<CComSingleThreadModel>,
    public IDispatchImpl<IWebBrowserEventHandler, &IID_IWebBrowserEventHandler, &LIBID_AnchoBgSrvLib,
                /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
  DECLARE_NOT_AGGREGATABLE(CPopupResizeEventHandler)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CPopupResizeEventHandler)
    COM_INTERFACE_ENTRY(IWebBrowserEventHandler)
    COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()

public:
  CPopupResizeEventHandler(){}
  // -------------------------------------------------------------------------
  // static creator function
  static HRESULT createObject(CPopupWindow *aWin, CPopupResizeEventHandlerComObject *& pRet)
  {
    CPopupResizeEventHandlerComObject *newObject = pRet = NULL;
    IF_FAILED_RET(CPopupResizeEventHandlerComObject::CreateInstance(&newObject));
    newObject->AddRef();
    newObject->mWin = aWin;
    pRet = newObject;
    return S_OK;
  }

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct(){return S_OK;}
  void FinalRelease(){}


  STDMETHOD(onFire)()
  {
    mWin->checkResize();
    return S_OK;
  }

private:
  CPopupWindow *mWin;
};

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------



HRESULT CPopupWindow::FinalConstruct()
{
  m_WebBrowserEventsCookie = 0;
  CComPtr<CPopupResizeEventHandlerComObject> eventHandler;
  CPopupResizeEventHandler::createObject(this, eventHandler.p);
  mEventHandler = eventHandler;
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
  checkResize();

  CComQIPtr<IHTMLElement2> bodyElement = getBodyElement();

  if (bodyElement) {
    bodyElement->put_onresize(mEventHandler);
  }
}

void CPopupWindow::checkResize()
{
  CComQIPtr<IHTMLElement2> bodyElement = getBodyElement();
  if (!bodyElement) {
    return;
  }

  long contentHeight, contentWidth;
  if (FAILED(bodyElement->get_scrollHeight(&contentHeight)) ||
    FAILED(bodyElement->get_scrollWidth(&contentWidth)))
  {
    return;
  }
  if (contentHeight > 0 && contentWidth > 0) {
    CRect rect;
    BOOL res = GetWindowRect(rect);

    if (res && (rect.Height() != contentHeight || rect.Width() != contentWidth)) {
      MoveWindow(rect.left, rect.top, contentWidth, contentHeight, TRUE);
    }
  }
}

CComPtr<IHTMLElement> CPopupWindow::getBodyElement()
{
  CComPtr<IDispatch> doc;
  if (FAILED(m_pWebBrowser->get_Document(&doc)) || !doc) {
    return CComPtr<IHTMLElement>();
  }
  CComQIPtr<IHTMLDocument2> htmlDocument2 = doc;
  if (!htmlDocument2) {
    return CComPtr<IHTMLElement>();
  }
  CComPtr<IHTMLElement> element;
  if (FAILED(htmlDocument2->get_body(&element)) || !element ) {
    return CComPtr<IHTMLElement>();
  }
  return element;
}


HRESULT CPopupWindow::CreatePopupWindow(HWND aParent, const DispatchMap &aInjectedObjects, LPCWSTR aURL, int aX, int aY, CIDispatchHelper aCloseCallback)
{
  CPopupWindowComObject * pNewWindow = NULL;
  IF_FAILED_RET(CPopupWindowComObject::CreateInstance(&pNewWindow));
  pNewWindow->m_sURL = aURL;
  pNewWindow->m_InjectedObjects = aInjectedObjects;
  pNewWindow->m_CloseCallback = aCloseCallback;
  RECT r = {aX, aY, aX + defaultWidth, aY + defaultHeight};

  if (!pNewWindow->Create(aParent, r, NULL, WS_POPUP))
  {
    return E_FAIL;
  }
  pNewWindow->ShowWindow(SW_SHOW);
  return S_OK;
}