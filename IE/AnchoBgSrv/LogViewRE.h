#pragma once

#ifndef USE_HTML_LOGWINDOW

class CLogView : public CWindowImpl<CLogView, CRichEditCtrl>
{
public:
  void Log(LogFacility logType, BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  void Log(LogFacility logType, BSTR bsSource, BSTR bsModuleID, VARIANT vtValue);
  void ClearLog();

public:
  DECLARE_WND_SUPERCLASS(NULL, CRichEditCtrl::GetWndClassName())

  BEGIN_MSG_MAP(CLogView)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
  END_MSG_MAP()

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
  void LogIntro(LogFacility logType, BSTR bsSource, BSTR bsModuleID);
  void LogVariant(VARIANT & value);
  void LogExtro();
};

#endif // #ndef USE_HTML_LOGWINDOW
