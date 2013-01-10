#pragma once

#include <atlwin.h>
#include "HtmlPanel.h"

struct ToolbarCallback {
  virtual void ToolbarWindowBeforeNavigate() = 0;
  virtual void ToolbarWindowReady(VARIANT *pURL) = 0;
  virtual void ToolbarWindowNavigateComplete(IDispatch *pDisp, VARIANT *URL) = 0;
  virtual void ToolbarWindowOnSetFocus() = 0;
  virtual void ToolbarWindowOnKillFocus() = 0;
};

class CHtmlToolbarWindow : public CHtmlPanel
{
public:
  CHtmlToolbarWindow();

  ToolbarCallback *m_toolbarCallback;

  virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  HRESULT setExternalObject(IDispatch *aExternal)
  {
    if (!aExternal) {
      return E_POINTER;
    }
    m_External = aExternal;
    return S_OK;
  }

  void setTabId(int aTabId) {
    mTabId = aTabId;
  }

protected:
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  virtual void OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);
  virtual void OnBrowserDocumentComplete(LPDISPATCH pDisp, VARIANT *URL);
  virtual void OnBrowserWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel);
  virtual void OnBrowserNavigateComplete(IDispatch *pDisp, VARIANT *URL);
  virtual void OnBrowserSetFocus();
  virtual void OnBrowserKillFocus();

  int mTabId;
};
