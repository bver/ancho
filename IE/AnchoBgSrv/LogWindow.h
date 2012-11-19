#pragma once

#include "resource.h"

#ifdef USE_HTML_LOGWINDOW
#include "LogView.h"
#else
#include "LogViewRE.h"
#endif
#include "AnchoBgSrv_i.h"

class CLogWindow;
typedef CComObject<CLogWindow>  CLogWindowComObject;

class CLogWindow :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CFrameWindowImpl<CLogWindow>,
  public IAnchoBackgroundConsole
{
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  virtual void OnFinalMessage(HWND);

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(CLogWindow)
    COM_INTERFACE_ENTRY(IAnchoBackgroundConsole)
  END_COM_MAP()

  BEGIN_MSG_MAP(CLogWindow)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_CLOSE, OnClose)
    CHAIN_MSG_MAP(CFrameWindowImpl<CLogWindow>)
  END_MSG_MAP()

  HRESULT FinalConstruct();
  void FinalRelease();

  static HRESULT CreateLogWindow(CLogWindowComObject ** ppRet);

  // Handler prototypes (uncomment arguments if needed):
//  LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//  LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//  LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
  LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
  STDMETHOD(log)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD(debug)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD(info)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD(warn)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD(error)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);

private:
  CLogView m_view;
};

