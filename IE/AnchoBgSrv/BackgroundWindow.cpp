#include "StdAfx.h"
#include "BackgroundWindow.h"
#include "XmlHttpRequest.h"

HRESULT CBackgroundWindow::FinalConstruct()
{
  return S_OK;
}

void CBackgroundWindow::FinalRelease()
{
  int asd = 0;
}

void CBackgroundWindow::OnFinalMessage(HWND)
{
  // This Release call is paired with the AddRef call in OnCreate.
  Release();
}

LRESULT CBackgroundWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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
  CIDispatchHelper script = CIDispatchHelper::GetScriptDispatch(m_pWebBrowser);
  for (DispatchMap::iterator it = m_InjectedObjects.begin(); it != m_InjectedObjects.end(); ++it) {
    script.SetProperty((LPOLESTR)(it->first.c_str()), CComVariant(it->second));
  }
  CIDispatchHelper window;
  script.Get<CIDispatchHelper, VT_DISPATCH, IDispatch*>(L"window", window);
  if (window) {
    CAnchoXmlHttpRequestComObject *requestFactory;
    IF_FAILED_RET(CAnchoXmlHttpRequestComObject::CreateInstance(&requestFactory));
    IDispatchEx *dispEx = NULL;
    requestFactory->QueryInterface<IDispatchEx>(&dispEx);
    CIDispatchHelper requestConstructor = dispEx;
    IF_FAILED_RET(window.SetProperty((LPOLESTR)L"XMLHttpRequest", CComVariant(requestConstructor.p)));

    /*CAnchoXmlHttpRequestFactoryComObject *requestFactory;
    IF_FAILED_RET(CAnchoXmlHttpRequestFactoryComObject::CreateInstance(&requestFactory));
    CIDispatchHelper requestConstructor = requestFactory;
    IF_FAILED_RET(window.SetProperty((LPOLESTR)L"XMLHttpRequest", CComVariant(requestConstructor.p)));*/
//    IF_FAILED_RET(window.SetProperty((LPOLESTR)L"ActiveXObject", CComVariant(requestConstructor.p)));
    //script.Get<CIDispatchHelper, VT_DISPATCH, IDispatch*>(L"XMLHttpRequest", requestConstructor);
  }



  // This AddRef call is paired with the Release call in OnFinalMessage
  // to keep the object alive as long as the window exists.
  AddRef();
  return 0;
}

LRESULT CBackgroundWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
  bHandled = FALSE;
  m_pWebBrowser.Release();
//  m_pDispApiJS.Release();
  return 1;
}

HRESULT CBackgroundWindow::CreateBackgroundWindow(const DispatchMap &aInjectedObjects, LPCWSTR lpszURL, CBackgroundWindowComObject ** ppRet)
{
  ENSURE_RETVAL(ppRet);
  (*ppRet) = NULL;
  CBackgroundWindowComObject * pNewWindow = NULL;
  IF_FAILED_RET(CBackgroundWindowComObject::CreateInstance(&pNewWindow));
  pNewWindow->AddRef();
  pNewWindow->m_sURL = lpszURL;
  pNewWindow->m_InjectedObjects = aInjectedObjects;
  RECT r = {0,0,0,0};
  if (!pNewWindow->Create(NULL, r, NULL, WS_POPUP))
  {
    pNewWindow->Release();
    return E_FAIL;
  }
  pNewWindow->ShowWindow(SW_HIDE);
  (*ppRet) = pNewWindow;
  return S_OK;
}
