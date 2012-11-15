#pragma once

#include <Shlobj.h>
#include "HTMLToolbarWindow.h"
#include "AnchoBgSrv_i.h"

inline CComQIPtr<IHTMLWindow2> IWebBrowserToIHTMLWindow(IWebBrowser2* spBrowser)
{
  ATLASSERT(spBrowser != NULL);
  CComQIPtr<IHTMLWindow2> spWindow;

  // Get the document of the browser.
  CComQIPtr<IDispatch> spDisp;
  spBrowser->get_Document(&spDisp);

  if (spDisp)
  {
    // Get the window of the document.
    CComQIPtr<IHTMLDocument2> spDoc = spDisp;
    if (spDoc != NULL)
    {
      spDoc->get_parentWindow(&spWindow);
    }
  }

  if (NULL == spWindow)
  {
    spWindow = spBrowser;
  }

  return spWindow;
}


class ATL_NO_VTABLE CToolbar :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CToolbar, &CLSID_IEToolbar>,
  public IObjectWithSiteImpl<CToolbar>,
  public IDeskBand,
  public IInputObject,
  public ToolbarCallback
{
public:
  DECLARE_REGISTRY_RESOURCEID(IDR_ANCHOTOOLBAR)

  // these members have to be public, they have to be accessible from CSalsitaScriptedClient
  HWND m_hWndParent;
  CString      m_sRootURL;

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(CToolbar)
    COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
    COM_INTERFACE_ENTRY(IOleWindow)
    COM_INTERFACE_ENTRY_IID(IID_IDockingWindow, IDockingWindow)
    COM_INTERFACE_ENTRY(IInputObject)
  END_COM_MAP()

private:
  std::wstring m_PageUrl;

protected:
  CComObject<CHtmlToolbarWindow> *m_HtmlWindow;
  CComPtr<IInputObjectSite> m_InputObjectSite;

  CToolbar() : m_dwBandID(0), m_dwViewMode(0), m_hWndParent(NULL)
  {
  }

  DWORD m_dwBandID, m_dwViewMode;
  /**
   * Fills in values for IDeskBand::GetBandInfo.
   */
  virtual void GetBandInfoValues(const wchar_t *& title, POINTL &minSize);

  /**
   * Retrieves url for the toolbar html page.
   * An url to local file (in the resources directory) can be constructed using CSalsitaExtensionHelper::ResourcesDirMakeUrl helper function.
   */
  //virtual void GetToolbarPage(std::wstring &pageUrl) = 0;

protected:
  // CSalsitaScriptedClient
  virtual HRESULT GetWindowForScript(IWebBrowser2* pWebBrowser, CComQIPtr<IHTMLWindow2> &window)
  {
    window = IWebBrowserToIHTMLWindow(m_HtmlWindow->m_pWebBrowser);
    if (!window)
    {
      return E_FAIL;
    } else {
      return S_OK;
    }
  }

public:
  HRESULT FinalConstruct()
  {
    HRESULT hr = CComObject<CHtmlToolbarWindow>::CreateInstance(&m_HtmlWindow);
    if (FAILED(hr))
    {
      return hr;
    }
    m_HtmlWindow->AddRef();
    m_HtmlWindow->m_toolbarCallback = dynamic_cast<ToolbarCallback *>(this);
    return hr;
  }

  void FinalRelease()
  {
    if (m_HtmlWindow)
    {
      m_HtmlWindow->m_toolbarCallback = NULL;
      m_HtmlWindow->Release();
      m_HtmlWindow = NULL;
    }
  }

  // IObjectWithSite
public:
  STDMETHODIMP SetSite(IUnknown *pUnkSite)
  {
    m_spUnkSite = pUnkSite;

    if (m_spUnkSite)
    {
      HRESULT hr = InternalSetSite();
      ATLASSERT(hr == S_OK);
    }
    else
    {
      HRESULT hr = InternalReleaseSite();
      ATLASSERT(hr == S_OK);
    }
    return S_OK;
  }

  virtual void GetBrowserSite(CComPtr<IUnknown> &site)
  {
    site = m_spUnkSite;
  }

private:

  HRESULT RunToolbarPage()
  {
    /*GetToolbarPage(m_PageUrl);
    std::wstring rootURL;
    CSalsitaExtensionHelper::ResourcesDirMakeUrl(m_ResourcesDir.c_str(), L"", rootURL);
    m_sRootURL = _T("salsita://") + CString(GetExtensionId()) + _T("/");
    if (m_sRootURL.Right(1) == _T("/"))
    {
      m_sRootURL = m_sRootURL.Left(m_sRootURL.GetLength()-1);
    }*/

    m_HtmlWindow->Create(m_hWndParent, CWindow::rcDefault, L"about:blank",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);

    if (!m_HtmlWindow->m_pWebBrowser)
    {
      return E_FAIL;
    }
    m_hWndParent = m_HtmlWindow->m_hWnd;

    return m_HtmlWindow->m_pWebBrowser->Navigate(CComBSTR(L"http://err.f"), NULL, NULL, NULL, NULL);
    
  }

  virtual void ToolbarWindowBeforeNavigate()
  {
    //DestroyMagpieInstance(m_Magpie);
  }

  virtual void ToolbarWindowReady(VARIANT *pURL)
  {
    CString url(pURL->bstrVal);
    if (url != _T("about:blank"))
    {
      /*HRESULT hr = ExecuteContentScript();
      ATLASSERT(SUCCEEDED(hr));*/
    }
  }

  virtual void ToolbarWindowNavigateComplete(IDispatch *pDisp, VARIANT *URL)
  {
    /*if (!URL || URL->vt != VT_BSTR)
    {
      return;
    }
    CString url(URL->bstrVal);
    if (url == _T("about:blank"))
    {
      return;
    }
    HRESULT hr;
    hr = ConnectToBackgroundScript(&m_ScriptServiceInstance.p);
    ATLASSERT(SUCCEEDED(hr));

    hr = ReloadContentScript(m_Magpie, m_HtmlWindow->m_pWebBrowser);
    ATLASSERT(SUCCEEDED(hr));

    // If the user just enabled the addon then the existing tabs won't be attached
    // to the BHO, so we have to run the background scripts here.
    hr = m_ScriptServiceInstance->RunBackgroundScripts();
    ATLASSERT(SUCCEEDED(hr));

    // Even though we're not really a tab, we still need a "tab ID".
    // It's the unique key used to identify our listeners.
    // We pass in an empty window and browser to indicate that we are not a real tab.
    hr = m_ScriptServiceInstance->RegisterTab(0, NULL, &m_TabId);
    ATLASSERT(SUCCEEDED(hr));

    if (url.Left(10) == L"salsita://") {
      // Load this module first so it becomes the root module.
      // This enables relative paths to load properly.
      hr = m_Magpie->Run(L"js/popupDummy");
      ATLASSERT(SUCCEEDED(hr));

      // Load in require and salsita symbols from Magpie.
      CIDispatchHelper helper = CIDispatchHelper::GetScriptDispatch(m_HtmlWindow->m_pWebBrowser);
      CComPtr<IMagpieModule> rootModule;
      HRESULT hr = m_Magpie->GetRootModule(&rootModule.p);
      if (SUCCEEDED(hr))
      {
        CComPtr<IMagpieRequire> require;
        rootModule->GetRequire(&require.p);
        CComPtr<IMagpieRequire> requireWrapper;
        CMagpieRequireWrapper::CreateObject(require, m_HtmlWindow->m_pWebBrowser, m_Magpie, &requireWrapper);
        if (SUCCEEDED(hr))
        {
          hr = helper.SetProperty(L"require", CComVariant(requireWrapper));
          ATLASSERT(SUCCEEDED(hr));
        }
      }

      CComVariant vSalsitaApi;
      hr = m_Magpie->GetSalsitaObject(&vSalsitaApi);
      if (SUCCEEDED(hr)) {
        hr = helper.SetPropertyByRef(L"salsita", vSalsitaApi);
        if (FAILED(hr)) {
          hr = helper.SetProperty(L"salsita", vSalsitaApi);
        }
        ATLASSERT(SUCCEEDED(hr));
      }
    }*/
  }

  virtual void ToolbarWindowOnSetFocus()
  {
    HRESULT hr;
    if (m_InputObjectSite) {
      CComPtr<IUnknown> pUnk;
      if (SUCCEEDED(QueryInterface(IID_IUnknown, (LPVOID*) &pUnk.p)))
      {
        hr = m_InputObjectSite->OnFocusChangeIS(pUnk, TRUE);
        ATLASSERT(SUCCEEDED(hr));
      }
    }
  }

  virtual void ToolbarWindowOnKillFocus()
  {
    HRESULT hr;
    // If an input field in the toolbar has the focus when the user clicks on the main
    // browser window, the caret stays in the input field instead of disappearing. Sending
    // WM_UPDATEUISTATUS manually to this window doesn't seem to help, so we simply
    // change the active element to make sure that the caret disappears.
    CComQIPtr<IDispatch> documentDispatch;
    if (m_HtmlWindow && SUCCEEDED(m_HtmlWindow->m_pWebBrowser->get_Document(&documentDispatch))) {
      CComQIPtr<IHTMLDocument3> document(documentDispatch);
      if (document) {
        CComQIPtr<IHTMLElement> element;
        if (SUCCEEDED(document->get_documentElement(&element))) {
          CComQIPtr<IHTMLElement3> element3(element);
          if (element3) {
            element3->setActive();
          }
        }
      }
    }

    if (m_InputObjectSite) {
      CComPtr<IUnknown> pUnk;
      if (SUCCEEDED(QueryInterface(IID_IUnknown, (LPVOID*) &pUnk.p)))
      {
        hr = m_InputObjectSite->OnFocusChangeIS(pUnk, FALSE);
        ATLASSERT(SUCCEEDED(hr));
      }
    }
  }

protected:

  virtual HRESULT InternalSetSite();

  virtual HRESULT InternalReleaseSite();

  // IDeskBand
public:
  STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi)
  {
    if (pdbi)
    {
      m_dwBandID = dwBandID;
      m_dwViewMode = dwViewMode;

      const wchar_t * title;
      POINTL minSize;
      GetBandInfoValues(title, minSize);

      if (pdbi->dwMask & DBIM_MINSIZE)
      {
        pdbi->ptMinSize.x = minSize.x;
        pdbi->ptMinSize.y = minSize.y;
      }

      if (pdbi->dwMask & DBIM_MAXSIZE)
      {
        pdbi->ptMaxSize.x = -1;
        pdbi->ptMaxSize.y = -1;
      }

      if (pdbi->dwMask & DBIM_INTEGRAL)
      {
        pdbi->ptIntegral.x = 1;
        pdbi->ptIntegral.y = 1;
      }

      if (pdbi->dwMask & DBIM_ACTUAL)
      {
        pdbi->ptActual.x = 0;
        pdbi->ptActual.y = 0;
      }

      if (pdbi->dwMask & DBIM_TITLE)
      {
        if (!title)
        {
          pdbi->dwMask &= ~DBIM_TITLE;
        } else {
          wcscpy_s(pdbi->wszTitle, _countof(pdbi->wszTitle), title);
        }
      }

      if (pdbi->dwMask & DBIM_MODEFLAGS)
      {
        pdbi->dwModeFlags = DBIMF_VARIABLEHEIGHT;
      }

      if (pdbi->dwMask & DBIM_BKCOLOR)
      {
        pdbi->dwMask &= ~DBIM_BKCOLOR;
      }

      return S_OK;
    }

    return E_INVALIDARG;
  }

  // IOleWindow
public:
  STDMETHOD(GetWindow)(HWND* phwnd)
  {
    if (!phwnd)
    {
      return E_POINTER;
    }

    *phwnd = m_HtmlWindow->m_hWnd;
    return S_OK;
  }

  STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode)
  {
    return S_OK;
  }

  // IDockingWindow
public:
  STDMETHOD(CloseDW)(unsigned long dwReserved)
  {
    return ShowDW(FALSE);
  }

  STDMETHOD(ResizeBorderDW)(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(ShowDW)(BOOL fShow)
  {
    m_HtmlWindow->ShowWindow(fShow ? SW_SHOW : SW_HIDE);
    return S_OK;
  }

  // IInputObject
public:
  STDMETHOD(HasFocusIO)(void)
  {
    HWND hFocusWnd = ::GetFocus();
    return ::IsChild(m_HtmlWindow->m_hWnd, hFocusWnd) ? S_OK : S_FALSE;
  }

  STDMETHOD(TranslateAcceleratorIO)(LPMSG lpMsg)
  {
    CComQIPtr<IOleInPlaceActiveObject, &IID_IOleInPlaceActiveObject> pIOIPAO(m_HtmlWindow->m_pWebBrowser);
    if (!pIOIPAO || pIOIPAO->TranslateAccelerator(lpMsg) != S_OK) {
      TranslateMessage(lpMsg);
      DispatchMessage(lpMsg);
    }

    return S_OK;
  }

  STDMETHOD(UIActivateIO)(BOOL fActivate, LPMSG lpMsg)
  {
    if (fActivate)
    {
      m_HtmlWindow->SetFocus();
    }
    return S_OK;
  }
};


OBJECT_ENTRY_AUTO(CLSID_IEToolbar, CToolbar)