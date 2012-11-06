#pragma once

#include "resource.h"

#include "AnchoBgSrv_i.h"

class CBackgroundWindow;
typedef CComObject<CBackgroundWindow>  CBackgroundWindowComObject;

class CBackgroundWindow :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CWindowImpl<CBackgroundWindow, CAxWindow>,
  public IUnknown
{
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  virtual void OnFinalMessage(HWND);

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(CBackgroundWindow)
    COM_INTERFACE_ENTRY(IUnknown)
  END_COM_MAP()

  BEGIN_MSG_MAP(CBackgroundWindow)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  END_MSG_MAP()

  HRESULT FinalConstruct();
  void FinalRelease();

  static HRESULT CreateBackgroundWindow(const DispatchMap &aInjectedObjects, LPCWSTR lpszURL, CBackgroundWindowComObject ** ppRet);

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

private:
  CComQIPtr<IWebBrowser2>   m_pWebBrowser;     // Embedded WebBrowserControl
  DispatchMap m_InjectedObjects;
  CStringW    m_sURL;
};
