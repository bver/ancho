#pragma once

#include <Shlobj.h>
#include "HTMLToolbarWindow.h"
#include "ancho_i.h"
#include <string>

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
class ATL_NO_VTABLE CBasicToolbar :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TImplementation, tClsid>,
  public IObjectWithSiteImpl<TImplementation>,
  public IDeskBand,
  public IInputObject
{
public:
  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(TImplementation)
    COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
    COM_INTERFACE_ENTRY(IOleWindow)
    COM_INTERFACE_ENTRY_IID(IID_IDockingWindow, IDockingWindow)
    COM_INTERFACE_ENTRY(IInputObject)
  END_COM_MAP()
public:
  HRESULT FinalConstruct()
  {
    ComContentWindow *contentWindow = NULL;
    HRESULT hr = ComContentWindow::CreateInstance(&contentWindow);
    if (FAILED(hr)) {
      return hr;
    }
    contentWindow->AddRef();
    mContentWindow.Attach(contentWindow);
    return hr;
  }

  void FinalRelease()
  {
  }

  STDMETHOD(SetSite)(IUnknown *pUnkSite);
  STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi);

  virtual void GetBandInfoValues(const wchar_t *& title, POINTL &minSize);
public:
  // IOleWindow
  STDMETHOD(GetWindow)(HWND* phwnd);
  STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

public:
  // IDockingWindow
  STDMETHOD(CloseDW)(unsigned long dwReserved);

  STDMETHOD(ResizeBorderDW)(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved);

  STDMETHOD(ShowDW)(BOOL fShow);

public:
  HWND findParentWindowByClass(std::wstring aClassName);

protected:
  virtual HRESULT InternalSetSite();
  virtual HRESULT InternalReleaseSite();

protected:
  typedef CComObject<CHtmlToolbarWindow> ComContentWindow;
  CComPtr<ComContentWindow> mContentWindow;

  CComPtr<IInputObjectSite> mInputObjectSite;

  HWND mHWNDParent;

  DWORD m_dwBandID, m_dwViewMode;
};

//*******************************************************************************

class CToolbar :
  public CBasicToolbar<CToolbar, CHtmlToolbarWindow, &CLSID_IEToolbar>,
  public ToolbarCallback
{
public:
  typedef CBasicToolbar<CToolbar, CHtmlToolbarWindow, &CLSID_IEToolbar> BaseClass;
  DECLARE_REGISTRY_RESOURCEID(IDR_ANCHOTOOLBAR)

  CToolbar(): mTabId(0)
  {
  }
  /**
   * Fills in values for IDeskBand::GetBandInfo.
   */
  virtual void GetBandInfoValues(const wchar_t *& title, POINTL &minSize);

public:
  HRESULT FinalConstruct()
  {
    HRESULT hr = BaseClass::FinalConstruct();
    if (FAILED(hr)) {
      return hr;
    }
    mContentWindow->m_toolbarCallback = dynamic_cast<ToolbarCallback *>(this);
    return hr;
  }

  void FinalRelease()
  {
    if (mContentWindow) {
      mContentWindow->m_toolbarCallback = NULL;
    }
    BaseClass::FinalRelease();
  }

  // IInputObject
  STDMETHOD(HasFocusIO)(void);
  STDMETHOD(TranslateAcceleratorIO)(LPMSG lpMsg);
  STDMETHOD(UIActivateIO)(BOOL fActivate, LPMSG lpMsg);

  virtual void ToolbarWindowBeforeNavigate();
  virtual void ToolbarWindowReady(VARIANT *pURL);
  virtual void ToolbarWindowNavigateComplete(IDispatch *pDisp, VARIANT *URL);
  virtual void ToolbarWindowOnSetFocus();
  virtual void ToolbarWindowOnKillFocus();
protected:
  virtual HRESULT InternalSetSite();
  virtual HRESULT InternalReleaseSite();

private:

  HRESULT RunToolbarPage();
  std::wstring mUrl;
  CComPtr<IAnchoAddonService> mAnchoService;
  int mTabId;

};

OBJECT_ENTRY_AUTO(CLSID_IEToolbar, CToolbar)

#include "Toolbar.ipp"