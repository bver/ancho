#pragma once

#ifdef USE_HTML_LOGWINDOW

class CLogView :
  public CWindowImpl<CLogView, CAxWindow>
{
public:
  void Log(LogFacility logType, BSTR bsSource, BSTR bsModuleID, VARIANT vtValue);
  void ClearLog();

  DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

  BEGIN_MSG_MAP(CLogView)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  END_MSG_MAP()

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
  CComPtr<IWebBrowser2>                   m_pWebBrowser;
};

#endif // #def USE_HTML_LOGWINDOW
