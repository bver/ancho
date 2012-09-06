/****************************************************************************
 * AnchoAddon.h : Declaration of the CAnchoAddon
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
 * class CAnchoAddon
 */
class CAnchoAddon;
typedef IDispEventImpl<1, CAnchoAddon, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0> DWebBrowserEvents2Ancho;
typedef CComObject<CAnchoAddon> CAnchoAddonComObject;

class ATL_NO_VTABLE CAnchoAddon :
  public CComObjectRootEx<CComSingleThreadModel>,
  public DWebBrowserEvents2Ancho,
  public IAnchoAddon
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CAnchoAddon() : m_InstanceID(0), m_dwAdviseSinkWebBrowser(0)
  {
  }

  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY()
  DECLARE_NOT_AGGREGATABLE(CAnchoAddon)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CAnchoAddon)
    COM_INTERFACE_ENTRY(IAnchoAddon)
  END_COM_MAP()

  // -------------------------------------------------------------------------
  // COM sink map
  BEGIN_SINK_MAP(CAnchoAddon)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, BrowserNavigateCompleteEvent)
  END_SINK_MAP()

  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
    Shutdown();
  }

public:
  // -------------------------------------------------------------------------
  // IAnchoAddon methods. See .idl for description.
  STDMETHOD(Init)(LPCOLESTR lpsExtensionID, IAnchoAddonService * pService, IWebBrowser2 * pWebBrowser);
  STDMETHOD(Shutdown)();

  // -------------------------------------------------------------------------
  // DWebBrowserEvents2 methods
  STDMETHOD_(void, BrowserNavigateCompleteEvent)(IDispatch *pDisp, VARIANT *URL);

private:
  // -------------------------------------------------------------------------
  // Private members.
  CString                               m_sExtensionName;
  CString                               m_sExtensionID;
  CString                               m_sExtensionPath;

  ULONG                                 m_InstanceID;

  CComQIPtr<IWebBrowser2>               m_pWebBrowser;
  DWORD                                 m_dwAdviseSinkWebBrowser;     // Cookie for AtlAdvise

  CComPtr<IAnchoAddonService>           m_pAnchoService;
  CComPtr<IAnchoAddonBackground>        m_pAddonBackground;
  CComQIPtr<IAnchoBackgroundConsole>    m_pBackgroundConsole;
  CComQIPtr<IDispatch>                  m_pContentAPI;

  CComPtr<IMagpieApplication>           m_Magpie;

};

//OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(AnchoAddon), CAnchoAddon)
