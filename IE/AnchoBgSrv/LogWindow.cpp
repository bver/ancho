#include "StdAfx.h"
#include "LogWindow.h"
#include "AnchoBgSrvModule.h"

HRESULT CLogWindow::FinalConstruct()
{
  return S_OK;
}

void CLogWindow::FinalRelease()
{
  int asd = 0;
}

void CLogWindow::OnFinalMessage(HWND)
{
  // This Release call is paired with the AddRef call in OnCreate.
  Release();
}

LRESULT CLogWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  // remove old menu
  SetMenu(NULL);

#ifdef USE_HTML_LOGWINDOW
  CString s, sm;
  GetModuleFileName(_AtlModule.GetResourceInstance(), sm.GetBuffer(_MAX_PATH), _MAX_PATH);
  sm.ReleaseBuffer();
  s.Format(_T("res://%s/log.html"), sm);
  m_hWndClient = m_view.Create(m_hWnd, rcDefault, s, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL);
#else
  m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_SAVESEL);
#endif // #def USE_HTML_LOGWINDOW

  // This AddRef call is paired with the Release call in OnFinalMessage
  // to keep the object alive as long as the window exists.
  AddRef();
  return 0;
}

LRESULT CLogWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
  bHandled = FALSE;
  return 1;
}

LRESULT CLogWindow::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  ShowWindow(SW_SHOWMINIMIZED);
  return 0;
}

HRESULT CLogWindow::CreateLogWindow(CLogWindowComObject ** ppRet)
{
  ENSURE_RETVAL(ppRet);
  (*ppRet) = NULL;
  CLogWindowComObject * pNewWindow = NULL;
  IF_FAILED_RET(CLogWindowComObject::CreateInstance(&pNewWindow));
  pNewWindow->AddRef();
  if (!pNewWindow->CreateEx())
  {
    pNewWindow->Release();
    return E_FAIL;
  }
  pNewWindow->ShowWindow(SW_SHOW);
  (*ppRet) = pNewWindow;
  return S_OK;
}

STDMETHODIMP CLogWindow::log(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals)
{
  m_view.Log(LT_LOG, bsSource, bsModuleID, pVals);
  return S_OK;
}

STDMETHODIMP CLogWindow::debug(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals)
{
  m_view.Log(LT_DEBUG, bsSource, bsModuleID, pVals);
  return S_OK;
}

STDMETHODIMP CLogWindow::info(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals)
{
  m_view.Log(LT_INFO, bsSource, bsModuleID, pVals);
  return S_OK;
}

STDMETHODIMP CLogWindow::warn(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals)
{
  m_view.Log(LT_WARN, bsSource, bsModuleID, pVals);
  return S_OK;
}

STDMETHODIMP CLogWindow::error(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals)
{
  m_view.Log(LT_ERROR, bsSource, bsModuleID, pVals);
  return S_OK;
}
