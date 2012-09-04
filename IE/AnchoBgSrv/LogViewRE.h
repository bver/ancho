#pragma once

#ifndef USE_HTML_LOGWINDOW

class CLogView : public CWindowImpl<CLogView, CRichEditCtrl>
{
public:
  void Log(LogFacility logType, BSTR bsSource, BSTR bsModuleID, VARIANT vtValue);
  void ClearLog();

public:
  DECLARE_WND_SUPERCLASS(NULL, CRichEditCtrl::GetWndClassName())

  BEGIN_MSG_MAP(CLogView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
  END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

#endif // #ndef USE_HTML_LOGWINDOW
