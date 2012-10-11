/****************************************************************************
 * AnchoRuntime.h : Declaration of the CAnchoRuntime
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once
#include "resource.h"       // main symbols

#include "ancho_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

/*============================================================================
 * class CAnchoRuntime
 */
class ATL_NO_VTABLE CAnchoRuntime :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CAnchoRuntime, &CLSID_AnchoRuntime>,
  public IObjectWithSiteImpl<CAnchoRuntime>,
  public IAnchoRuntime
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CAnchoRuntime()
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
  // COM standard methods
  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
    DestroyAddons();
  }

public:
  // -------------------------------------------------------------------------
  // IObjectWithSiteImpl methods
  STDMETHOD(SetSite)(IUnknown *pUnkSite);

private:
  // -------------------------------------------------------------------------
  // Methods
  HRESULT InitAddons();
  void DestroyAddons();

private:
  // -------------------------------------------------------------------------
  // Private members.
  CComQIPtr<IWebBrowser2>                 m_pWebBrowser;
  CComPtr<IAnchoAddonService>             m_pAnchoService;
  CAtlMap<CStringW, CComPtr<IAnchoAddon> > m_Addons;

  int m_TabID;
};

OBJECT_ENTRY_AUTO(__uuidof(AnchoRuntime), CAnchoRuntime)
