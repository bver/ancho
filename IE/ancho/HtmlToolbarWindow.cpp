#include "stdafx.h"
#include "HtmlToolbarWindow.h"

static TCHAR* PROPINST = TEXT("_CHtmlToolbarWindow_INST");
static TCHAR* PROPPROC = TEXT("_CHtmlToolbarWindow_PROC");

LRESULT CHtmlToolbarWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CHtmlToolbarWindow* pThis = (CHtmlToolbarWindow *) ::GetProp(hwnd, PROPINST);
  WNDPROC fnOldWndProc = (WNDPROC) ::GetProp(hwnd, PROPPROC);

  if (uMsg == WM_SETFOCUS) {
    pThis->OnBrowserSetFocus();
  }
  else if (uMsg == WM_KILLFOCUS) {
    pThis->OnBrowserKillFocus();
  }
  return fnOldWndProc(hwnd, uMsg, wParam, lParam);
}

CHtmlToolbarWindow::CHtmlToolbarWindow() : m_toolbarCallback(NULL), mTabId(0)
{
}

LRESULT CHtmlToolbarWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  LRESULT ret = CHtmlPanel::OnCreate(uMsg, wParam, lParam, bHandled);
  if (ret){
    return ret;
  }
  return 0;
}

void CHtmlToolbarWindow::OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel)
{
  // We can't get the browser window directly from the IWebBrowser2 since it is a frame.
  HWND hBrowserWnd;
  IOleWindow* pOWin;
  HRESULT hr = m_pWebBrowser->QueryInterface(IID_IOleWindow, (void **)&pOWin);
  ATLASSERT(SUCCEEDED(hr));
  hr = pOWin->GetWindow(&hBrowserWnd);
  ATLASSERT(SUCCEEDED(hr));

  // We now have the Shell Embedding window. We need the child of its
  // child which is the actual browser window.
  hBrowserWnd = ::GetWindow(hBrowserWnd, GW_CHILD); // DocObject window
  ATLASSERT(hBrowserWnd != NULL);
  hBrowserWnd = ::GetWindow(hBrowserWnd, GW_CHILD); // Yay, the browser window!

  // Check if the window is already subclassed.
  if (hBrowserWnd && !::GetProp(hBrowserWnd, PROPPROC)) {
    // Subclass the window so we can handle the focus messages.
    WNDPROC fnOldWndProc =
      (WNDPROC) ::SetWindowLongPtrW(hBrowserWnd, GWL_WNDPROC, (LONG_PTR) WindowProc);
    ::SetProp(hBrowserWnd, PROPPROC, fnOldWndProc);
    ::SetProp(hBrowserWnd, PROPINST, this);
  }

  if (m_toolbarCallback)
  {
    m_toolbarCallback->ToolbarWindowBeforeNavigate();
  }
}

void CHtmlToolbarWindow::OnBrowserDocumentComplete(LPDISPATCH pDispatch, VARIANT *URL)
{
  // Make sure that we aren't zoomed (since the web browser control is initialized with the zoom
  // of the browser window).
  CComVariant zoom(100);
  CComVariant result;
  CComPtr<IWebBrowser2> pWebBrowser;
  if (SUCCEEDED(pDispatch->QueryInterface(&pWebBrowser))) {

    HRESULT hr = pWebBrowser->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &zoom, &result);

    if (m_toolbarCallback)
    {
      m_toolbarCallback->ToolbarWindowReady(URL);
    }
  }
  CIDispatchHelper scriptDispatch = CIDispatchHelper::GetScriptDispatch(m_pWebBrowser);
  if (scriptDispatch) {
    CComVariant idVariant = mTabId;
    DISPPARAMS params = {&idVariant, NULL, 1, 0};
    scriptDispatch.Call((LPOLESTR)L"initBrowserActionPage", &params);
  }
}

void CHtmlToolbarWindow::OnBrowserWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel)
{
  if (Cancel)
  {
    *Cancel = TRUE;
  }
}

void CHtmlToolbarWindow::OnBrowserNavigateComplete(IDispatch *pDisp, VARIANT *URL)
{
  if (m_toolbarCallback)
  {
    m_toolbarCallback->ToolbarWindowNavigateComplete(pDisp, URL);
  }
}

void CHtmlToolbarWindow::OnBrowserSetFocus()
{
  if (m_toolbarCallback)
  {
    m_toolbarCallback->ToolbarWindowOnSetFocus();
  }
}

void CHtmlToolbarWindow::OnBrowserKillFocus()
{
  if (m_toolbarCallback)
  {
    m_toolbarCallback->ToolbarWindowOnKillFocus();
  }
}
