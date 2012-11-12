/****************************************************************************
 * AnchoBackground.h : Declaration of CAnchoAddonBackground
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 *
 * The whole API implementation is in the contained object m_BackgroundAPI.
 * This makes refcount handling a lot easier. Magpie inside CAnchoBackgroundAPI
 * holds some references to CAnchoBackgroundAPI, while clients of this class
 * hold references to it. By separating the part held by the client from the
 * part held by the scripting we can cleanly determine the point when the
 * last external reference got released (means the last client exists) and
 * perform our cleanup tasks. This is done via a call to
 * CAnchoBackgroundAPI::UnInit() - it stops the script engine and by this makes
 * the script engine release all internal references.
 ****************************************************************************/

#pragma once
#include "resource.h"       // main symbols

#include "AnchoBgSrv_i.h"
#include "AnchoBackgroundAPI.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

struct CAnchoAddonServiceCallback;
/*============================================================================
 * class CAnchoAddonBackground
 */
class ATL_NO_VTABLE CAnchoAddonBackground :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CAnchoAddonBackground, &CLSID_AnchoAddonBackground>,
  public IAnchoAddonBackground,
  public IAnchoBackgroundConsole
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CAnchoAddonBackground() : m_NextInstanceID(1)
  {
  }

public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY()
  DECLARE_NOT_AGGREGATABLE(CAnchoAddonBackground)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CAnchoAddonBackground)
    COM_INTERFACE_ENTRY(IAnchoAddonBackground)
    COM_INTERFACE_ENTRY(IAnchoBackgroundConsole)
  END_COM_MAP()

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
  void FinalRelease();

public:
  // -------------------------------------------------------------------------
  // Methods

  // Init initializes everything (loads the addon, manifest, magpie etc)
  HRESULT Init(LPCTSTR                    lpszThisPath,
               CAnchoAddonServiceCallback * pAddonServiceCallback,
               IAnchoServiceApi           * pServiceApi,
               BSTR                       bsID);

  // OnAddonServiceReleased is called from CAnchoAddonService::FinalRelease, means, when
  // CAnchoAddonService gets destroyed. It sets the callback pointer to NULL so
  // that CAnchoAddonService can not be called any more.
  // Since clients might still hold references to this object we can't simply
  // destroy everything here, so cleanup is done in FinalRelease().
  void OnAddonServiceReleased();

  STDMETHOD(invokeExternalEventObject)(BSTR aEventName, LPDISPATCH aArgs, VARIANT* aRet);
  STDMETHOD(invokeEventWithIDispatchArgument)(BSTR aEventName, LPDISPATCH aArg);
public:
  // -------------------------------------------------------------------------
  // IAnchoAddonBackground methods. See .idl for description.
  STDMETHOD(AdviseInstance)(ULONG * pulInstanceID);
  STDMETHOD(UnadviseInstance)(ULONG ulInstanceID);
  STDMETHOD(GetContentAPI)(ULONG ulInstanceID, LPDISPATCH* ppDisp);
  STDMETHOD(GetManifest)(LPDISPATCH* ppDisp);

  // -------------------------------------------------------------------------
  // IAnchoBackgroundConsole methods. See .idl for description.
  STDMETHOD(log)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD(debug)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD(info)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD(warn)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD(error)(BSTR bsSource, BSTR bsModuleID, SAFEARRAY* pVals);

private:
  // -------------------------------------------------------------------------
  // Private member functions

private:
  // -------------------------------------------------------------------------
  // Private variables

  // My id
  CComBSTR  m_bsID;

  // The scripting service. Have to notify him
  // when I'm about to destroy myself
  CAnchoAddonServiceCallback
          * m_pAddonServiceCallback;

  // The actual background API. See class description.
  CComObjectStackEx<CAnchoBackgroundAPI>
            m_BackgroundAPI;

  ULONG     m_NextInstanceID;

};
typedef CComObject<CAnchoAddonBackground> CAnchoAddonBackgroundComObject;


OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(AnchoAddonBackground), CAnchoAddonBackground)
