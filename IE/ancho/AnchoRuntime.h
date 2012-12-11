/****************************************************************************
 * AnchoRuntime.h : Declaration of the CAnchoRuntime
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once
#include "resource.h"       // main symbols

#include "ancho_i.h"
#include <map>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

/*============================================================================
 * class CAnchoRuntime
 */
class CAnchoRuntime;
typedef IDispEventImpl<1, CAnchoRuntime, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0> TWebBrowserEvents;
typedef IDispEventImpl<2, CAnchoRuntime, &IID_DAnchoBrowserEvents, &LIBID_anchoLib, 0xffff, 0xffff> TAnchoBrowserEvents;

class ATL_NO_VTABLE CAnchoRuntime :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CAnchoRuntime, &CLSID_AnchoRuntime>,
  public IObjectWithSiteImpl<CAnchoRuntime>,
  public TWebBrowserEvents,
  public TAnchoBrowserEvents,
  public IAnchoRuntime
{
  typedef std::map<std::wstring, CComPtr<IWebBrowser2> > FrameMap;
public:
  // -------------------------------------------------------------------------
  // ctor
  CAnchoRuntime() : m_WebBrowserEventsCookie(0), m_AnchoBrowserEventsCookie(0)
  {
  }

  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_REGISTRY_RESOURCEID(IDR_ANCHORUNTIME)
  DECLARE_NOT_AGGREGATABLE(CAnchoRuntime)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CAnchoRuntime)
    COM_INTERFACE_ENTRY(IAnchoRuntime)
    COM_INTERFACE_ENTRY(IObjectWithSite)
  END_COM_MAP()

  // -------------------------------------------------------------------------
  // COM sink map
  BEGIN_SINK_MAP(CAnchoRuntime)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBrowserBeforeNavigate2)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW3, OnNewWindow3)
    SINK_ENTRY_EX(2, IID_DAnchoBrowserEvents, 1, OnFrameStart)
    SINK_ENTRY_EX(2, IID_DAnchoBrowserEvents, 2, OnFrameEnd)
    SINK_ENTRY_EX(2, IID_DAnchoBrowserEvents, 3, OnFrameRedirect)
  END_SINK_MAP()

  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
    DestroyAddons();
    Cleanup();
  }

public:
  // -------------------------------------------------------------------------
  // IObjectWithSiteImpl methods
  STDMETHOD(SetSite)(IUnknown *pUnkSite);

  // -------------------------------------------------------------------------
  // IAnchoRuntime methods
  STDMETHOD(reloadTab)();
  STDMETHOD(closeTab)();
  STDMETHOD(executeScript)(BSTR aExtensionId, BSTR aCode, INT aFileSpecified);
  STDMETHOD(updateTab)(LPDISPATCH aProperties);
  STDMETHOD(fillTabInfo)(VARIANT* aInfo);

  // DWebBrowserEvents2 methods
  STDMETHOD_(void, OnBrowserBeforeNavigate2)(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags,
    VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);

  STDMETHOD_(void, OnNewWindow3)(IDispatch *pDisp, VARIANT_BOOL Cancel, DWORD dwFlags,	BSTR bstrUrlContext, BSTR bstrUrl);

  // -------------------------------------------------------------------------
  // DAnchoBrowserEvents methods.
  STDMETHOD(OnFrameStart)(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame);
  STDMETHOD(OnFrameEnd)(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame);
  STDMETHOD(OnFrameRedirect)(BSTR bstrOldUrl, BSTR bstrNewUrl);

private:
  // -------------------------------------------------------------------------
  // Methods
  HRESULT InitAddons();
  void DestroyAddons();
  HRESULT Init();
  HRESULT Cleanup();
  HRESULT InitializeContentScripting(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame, documentLoadPhase aPhase);

  HWND getTabWindow();
  HWND getFrameTabWindow()
    {return findParentWindowByClass(L"Frame Tab");}
  HWND getMainWindow()
    {return findParentWindowByClass(L"IEFrame");}

  HWND findParentWindowByClass(std::wstring aClassName);
  bool isTabActive();
private:
  // -------------------------------------------------------------------------
  // Private members.
  typedef std::map<std::wstring, CComPtr<IAnchoAddon> > AddonMap;
  CComQIPtr<IWebBrowser2>                 m_pWebBrowser;
  CComPtr<IAnchoAddonService>             m_pAnchoService;
  AddonMap                                m_Addons;
  int m_TabID;
  CComPtr<DAnchoBrowserEvents>            m_pBrowserEventSource;
  DWORD                                   m_WebBrowserEventsCookie;
  DWORD                                   m_AnchoBrowserEventsCookie;
  CComPtr<IClassFactory>                  m_CFHTTP;
  CComPtr<IClassFactory>                  m_CFHTTPS;
  FrameMap                                m_Frames;
};

OBJECT_ENTRY_AUTO(__uuidof(AnchoRuntime), CAnchoRuntime)
