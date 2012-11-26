#pragma once

#include <exdispid.h>


class CPopupWindow;
typedef CComObject<CPopupWindow>  CPopupWindowComObject;
typedef IDispEventImpl<1, CPopupWindow, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0> PopupWebBrowserEvents;

class CPopupWindow :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CWindowImpl<CPopupWindow, CAxWindow>,
  public IUnknown,
  public PopupWebBrowserEvents
{
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  virtual void OnFinalMessage(HWND);

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(CPopupWindow)
    COM_INTERFACE_ENTRY(IUnknown)
  END_COM_MAP()

  BEGIN_SINK_MAP(CPopupWindow)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
  END_SINK_MAP()

  BEGIN_MSG_MAP(CPopupWindow)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
  END_MSG_MAP()

  HRESULT FinalConstruct();
  void FinalRelease();

  static HRESULT CreatePopupWindow(HWND aParent, const DispatchMap &aInjectedObjects, LPCWSTR aURL, int aX, int aY, CIDispatchHelper aCloseCallback);

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
  LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  STDMETHOD_(void, OnDocumentComplete)(LPDISPATCH aDispatch, VARIANT *aURL);

private:
  CComQIPtr<IWebBrowser2>   m_pWebBrowser;     // Embedded WebBrowserControl
  DispatchMap m_InjectedObjects;
  CStringW    m_sURL;
  DWORD       m_WebBrowserEventsCookie;
  CIDispatchHelper m_CloseCallback;
};

