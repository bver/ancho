#pragma once

#include "HtmlPanelUIHandler.h"

#include <mshtmdid.h>
#include <ExDispid.h>

class CHtmlPanel;
typedef CWindowImpl<CHtmlPanel, CAxWindow> CHtmlPanelAxWindow;
typedef IDispEventImpl<1, CHtmlPanel, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0> DispWebBrowserEvents2;

class CHtmlPanel :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CHtmlPanel>,
  public CHtmlPanelAxWindow,
  public DispWebBrowserEvents2,
  public CHtmlPanelUIHandler<CHtmlPanel>
{
public:
  CComPtr<IWebBrowser2> m_pWebBrowser;
  DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

  BEGIN_MSG_MAP(CHtmlPanel)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
  END_MSG_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(CHtmlPanel)
    COM_INTERFACE_ENTRY(IDocHostUIHandlerDispatch)
  END_COM_MAP()

  BEGIN_SINK_MAP(CHtmlPanel)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, BrowserBeforeNavigate2Event)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, BrowserDocumentCompleteEvent)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_WINDOWCLOSING, BrowserWindowClosing)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, BrowserNavigateCompleteEvent)
  END_SINK_MAP()

  virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
  virtual LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

  STDMETHOD_(void, BrowserBeforeNavigate2Event)(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);
  STDMETHOD_(void, BrowserDocumentCompleteEvent)(IDispatch *pDisp, VARIANT *URL);
  STDMETHOD_(void, BrowserWindowClosing)(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel);
  STDMETHOD_(void, BrowserNavigateCompleteEvent)(IDispatch *pDisp, VARIANT *URL);

protected:
  CHtmlPanel();

  virtual void OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);
  virtual void OnBrowserDocumentComplete(LPDISPATCH pDisp, VARIANT *URL);
  virtual void OnBrowserWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel);
  virtual void OnBrowserNavigateComplete(IDispatch *pDisp, VARIANT *URL);
};
