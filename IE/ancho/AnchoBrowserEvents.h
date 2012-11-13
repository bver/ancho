/****************************************************************************
 * AnchoBrowserEvents.h : Declaration of the CAnchoBrowserEvents
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Matthew Gertner <matthew@salsitasoft.com>
 ****************************************************************************/

#pragma once
#include "ancho_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

 /*============================================================================
 * class CAnchoBrowserEvents
 */
class ATL_NO_VTABLE CAnchoBrowserEvents :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<DAnchoBrowserEvents, &IID_DAnchoBrowserEvents, &LIBID_anchoLib, 0xffff, 0xffff>,
  public IConnectionPointContainerImpl<CAnchoBrowserEvents>,
  public IConnectionPointImpl<CAnchoBrowserEvents, &IID_DAnchoBrowserEvents, CComDynamicUnkArray>
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CAnchoBrowserEvents()
  {
  }

  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY()
  DECLARE_NOT_AGGREGATABLE(CAnchoBrowserEvents)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CAnchoBrowserEvents)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(DAnchoBrowserEvents)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
  END_COM_MAP()

  // -------------------------------------------------------------------------
  // COM connection point map
  BEGIN_CONNECTION_POINT_MAP(CAnchoBrowserEvents)
    CONNECTION_POINT_ENTRY(IID_DAnchoBrowserEvents)
  END_CONNECTION_POINT_MAP()

public:
  // -------------------------------------------------------------------------
  // CAnchoBrowserEvents methods
  STDMETHOD(OnFrameStart)(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame);
  STDMETHOD(OnFrameEnd)(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame);
  STDMETHOD(OnFrameRedirect)(BSTR bstrOldUrl, BSTR bstrNewUrl);

private:
  // -------------------------------------------------------------------------
  // Methods
  HRESULT FireEvent(DISPID dispid, DISPPARAMS* disp, unsigned int count);

private:
  // -------------------------------------------------------------------------
  // Private members.
};
