#include "StdAfx.h"
#include "LogView.h"

#ifdef USE_HTML_LOGWINDOW

LRESULT CLogView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  DefWindowProc();

  CComPtr<IAxWinHostWindow> spHost;
  IF_FAILED_RET(QueryHost(__uuidof(IAxWinHostWindow), (void**)&spHost));

  spHost->QueryControl(IID_IWebBrowser2, (void**)&m_pWebBrowser.p);
  ATLASSERT(m_pWebBrowser);
  if (!m_pWebBrowser)
  {
    return -1;
  }
  return 0;
}

LRESULT CLogView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
  m_pWebBrowser.Release();
  bHandled = FALSE;
  return 1;
}

void CLogView::Log(LogFacility logType, BSTR bsSource, BSTR bsModuleID, VARIANT vtValue)
{
  if (!m_pWebBrowser)
  {
    return;
  }
  CComBSTR bsLogType;
  switch(logType)
  {
    case LT_ERROR:
      bsLogType = _T("error");
      break;
    case LT_WARN:
      bsLogType = _T("warn");
      break;
    case LT_INFO:
      bsLogType = _T("info");
      break;
    case LT_DEBUG:
      bsLogType = _T("debug");
      break;
    default:
      bsLogType = _T("log");
      break;
  }
  CIDispatchHelper script = CIDispatchHelper::GetScriptDispatch(m_pWebBrowser);
  if (!script)
  {
    return;
  }
  CComVariant vt[4] = {vtValue, bsModuleID, bsSource, bsLogType};
  DISPPARAMS params = {vt, NULL, 4, 0};
  script.Call(L"logMsg", &params);
}

void CLogView::ClearLog()
{
  if (!m_pWebBrowser)
  {
    return;
  }
  CIDispatchHelper script = CIDispatchHelper::GetScriptDispatch(m_pWebBrowser);
  if (!script)
  {
    return;
  }
  script.Call(L"clearLog");
}

#endif // #def USE_HTML_LOGWINDOW
