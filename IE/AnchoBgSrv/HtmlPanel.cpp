#include "stdafx.h"
#include "HtmlPanel.h"

//#include "libcomhelper\include\libcomhelper.h"
//using namespace LIB_COMHelper;

CHtmlPanel::CHtmlPanel()
{
}

LRESULT CHtmlPanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  DefWindowProc();

  CComPtr<IAxWinHostWindow> spHost;
  HRESULT hr = QueryHost(__uuidof(IAxWinHostWindow), (void**)&spHost);
  if (FAILED(hr))
    return -1;

  hr = spHost->QueryControl(IID_IWebBrowser2, (void**)&m_pWebBrowser.p);
  if (FAILED(hr))
    return -1;

  m_pWebBrowser->put_RegisterAsDropTarget(VARIANT_FALSE); ///< disable drag and drop to the page

  hr = DispWebBrowserEvents2::DispEventAdvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
  if (FAILED(hr))
    return -1;

  SetExternalUIHandler((IDocHostUIHandlerDispatch *)this);

  return 0;
}

LRESULT CHtmlPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  if (m_pWebBrowser)
  {
    DispWebBrowserEvents2::DispEventUnadvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
    m_pWebBrowser.Release();
  }
  return 0;
}

LRESULT CHtmlPanel::OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
  bHandled = FALSE;
  return 0;
}

STDMETHODIMP_(void) CHtmlPanel::BrowserBeforeNavigate2Event(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel)
{
  CComQIPtr<IWebBrowser2> caller = pDisp;
  if (caller && caller.IsEqualObject(m_pWebBrowser))
  {
    OnBrowserBeforeNavigate2(pDisp, pURL, Flags, TargetFrameName, PostData, Headers, Cancel);
  }
}

STDMETHODIMP_(void) CHtmlPanel::BrowserDocumentCompleteEvent(IDispatch *pDisp, VARIANT *URL)
{
  CComQIPtr<IWebBrowser2> caller = pDisp;
  if (caller && caller.IsEqualObject(m_pWebBrowser))
  {
    OnBrowserDocumentComplete(pDisp, URL);
  }
}

STDMETHODIMP_(void) CHtmlPanel::BrowserWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel)
{
  OnBrowserWindowClosing(IsChildWindow, Cancel);
}

STDMETHODIMP_(void) CHtmlPanel::BrowserNavigateCompleteEvent(IDispatch *pDisp, VARIANT *URL)
{
  CComQIPtr<IWebBrowser2> caller = pDisp;
  if (caller && caller.IsEqualObject(m_pWebBrowser))
  {
    OnBrowserNavigateComplete(pDisp, URL);
  }
}

void CHtmlPanel::OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel)
{
}

void CHtmlPanel::OnBrowserDocumentComplete(LPDISPATCH pDisp, VARIANT *URL)
{
}

void CHtmlPanel::OnBrowserWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel)
{
}

void CHtmlPanel::OnBrowserNavigateComplete(IDispatch *pDisp, VARIANT *URL)
{
}
