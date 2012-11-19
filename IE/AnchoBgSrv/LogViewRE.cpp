#include "StdAfx.h"
#include "LogViewRE.h"

#ifndef USE_HTML_LOGWINDOW

#define LOG_BGCOLOR         0x00ffffff
#define LOG_DEFAULTCOLOR    0
#define LOG_COLOR_LOG       0
#define LOG_COLOR_DEBUG     0x00f00000
#define LOG_COLOR_INFO      0x00008000
#define LOG_COLOR_WARN      0x000080a0
#define LOG_COLOR_ERROR     0x000000d0

LRESULT CLogView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  DefWindowProc();

  SetReadOnly();
  SetFont(AtlGetDefaultGuiFont());
  SetBackgroundColor(LOG_BGCOLOR);

  CHARFORMAT cf = {0};
  cf.cbSize = sizeof(CHARFORMAT);
  cf.crTextColor = LOG_DEFAULTCOLOR;
  cf.yHeight = 200;
  cf.dwMask = CFM_COLOR | CFM_SIZE;
  SetDefaultCharFormat(cf);
  return 0;
}

void CLogView::LogIntro(LogFacility logType, BSTR bsSource, BSTR bsModuleID)
{
  // set color, set bold, print date, logtype, source and module
  CHARFORMAT cf = {0};
  cf.cbSize = sizeof(CHARFORMAT);
  cf.dwMask = CFM_COLOR;
  CString sType;
  switch(logType)
  {
    case LT_DEBUG:
      sType = _T("debug");
      cf.crTextColor = LOG_COLOR_DEBUG;
      break;
    case LT_INFO:
      sType = _T("info");
      cf.crTextColor = LOG_COLOR_INFO;
      break;
    case LT_WARN:
      sType = _T("warning");
      cf.crTextColor = LOG_COLOR_WARN;
      break;
    case LT_ERROR:
      sType = _T("error");
      cf.crTextColor = LOG_COLOR_ERROR;
      break;
    default:
      sType = _T("log");
      cf.crTextColor = LOG_DEFAULTCOLOR;
      break;
  }

  CTime ts = CTime::GetCurrentTime();
  CString sDate(ts.Format(_T("%H:%M:%S")));

  CString s;
  s.Format(_T("%s %s [%s: %s]: "), sDate, sType, bsSource, bsModuleID);

  SetSelectionCharFormat(cf);
  AppendText(s);

  cf.dwMask |= CFM_BOLD;
  cf.dwEffects = CFE_BOLD;
  SetSelectionCharFormat(cf);
  // the view has now bold font with the correct color selected
}

void CLogView::LogExtro()
{
  // reset color
  CHARFORMAT cf = {0};
  cf.cbSize = sizeof(CHARFORMAT);
  cf.dwMask = CFM_COLOR;
  cf.crTextColor = LOG_DEFAULTCOLOR;
  SetSelectionCharFormat(cf);
  AppendText(_T("\r\n"));
}

void CLogView::LogVariant(VARIANT & value)
{
  CComVariant vt;
  vt.ChangeType(VT_BSTR, &value);
  if (vt.vt != VT_BSTR)
  {
    vt = _T("???");
  }
  AppendText(vt.bstrVal);
}

void CLogView::Log(LogFacility logType, BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals)
{
  LogIntro(logType, bsSource, bsModuleID);

  ATLASSERT(0 == pVals->rgsabound[0].lLbound);
  VARIANT* pVariants = (VARIANT*)pVals->pvData;
  for(ULONG n = 0; n < pVals->rgsabound[0].cElements; n++)
  {
    LogVariant(pVariants[pVals->rgsabound[0].cElements - n - 1]);
    AppendText(_T(" "));
  }

  LogExtro();
}

void CLogView::Log(LogFacility logType, BSTR bsSource, BSTR bsModuleID, VARIANT vtValue)
{
  LogIntro(logType, bsSource, bsModuleID);
  LogVariant(vtValue);
  LogExtro();
}

void CLogView::ClearLog()
{
  Clear();
}

#endif // #ndef USE_HTML_LOGWINDOW
