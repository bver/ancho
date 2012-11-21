/****************************************************************************
 * AnchoBackground.h : Declaration of CAnchoBackgroundAPI
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once
#include "resource.h"       // main symbols

#include "AnchoBgSrv_i.h"
#include "LogWindow.h"
#include "BackgroundWindow.h"

#include <list>
#include <vector>
#include <map>
#include <string>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

struct CAnchoAddonServiceCallback;

class CAnchoBackgroundAPI;
typedef IDispEventImpl<1, CAnchoBackgroundAPI, &DIID__IMagpieLoggerEvents, &LIBID_AnchoBgSrvLib, 0xffff, 0xffff> CAnchoAddonBackgroundLogger;

/*============================================================================
 * class CAnchoBackgroundAPI
 */
class ATL_NO_VTABLE CAnchoBackgroundAPI :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<IAnchoBackgroundAPI, &IID_IAnchoBackgroundAPI, &LIBID_AnchoBgSrvLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>,
  public CAnchoAddonBackgroundLogger
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CAnchoBackgroundAPI() : m_dwMagpieSinkCookie(0)
  {
  }

public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY()
  DECLARE_NOT_AGGREGATABLE(CAnchoBackgroundAPI)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CAnchoBackgroundAPI)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IAnchoBackgroundAPI)
  END_COM_MAP()

  BEGIN_SINK_MAP(CAnchoBackgroundAPI)
#ifdef MAGPIE_REGISTERED
    SINK_ENTRY_EX(1, DIID__IMagpieLoggerEvents, 1, OnLog)
    SINK_ENTRY_EX(1, DIID__IMagpieLoggerEvents, 2, OnDebug)
    SINK_ENTRY_EX(1, DIID__IMagpieLoggerEvents, 3, OnInfo)
    SINK_ENTRY_EX(1, DIID__IMagpieLoggerEvents, 4, OnWarn)
    SINK_ENTRY_EX(1, DIID__IMagpieLoggerEvents, 5, OnError)
#else
    SINK_ENTRY_INFO(1, DIID__IMagpieLoggerEvents, 1, OnLog, &info_Console_LogFunction)
    SINK_ENTRY_INFO(1, DIID__IMagpieLoggerEvents, 2, OnDebug, &info_Console_LogFunction)
    SINK_ENTRY_INFO(1, DIID__IMagpieLoggerEvents, 3, OnInfo, &info_Console_LogFunction)
    SINK_ENTRY_INFO(1, DIID__IMagpieLoggerEvents, 4, OnWarn, &info_Console_LogFunction)
    SINK_ENTRY_INFO(1, DIID__IMagpieLoggerEvents, 5, OnError, &info_Console_LogFunction)
#endif
  END_SINK_MAP()

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
  void FinalRelease();

public:
  // -------------------------------------------------------------------------
  // Methods

  // Init initializes everything (loads the addon, manifest, magpie etc)
  HRESULT Init(LPCTSTR lpszThisPath, LPCTSTR lpszRootURL, BSTR bsID, LPCTSTR lpszGUID, LPCTSTR lpszPath, IAnchoServiceApi *pServiceApi);

  // UnInit is called from CAnchoAddonBackground::FinalRelease, means, when
  // CAnchoAddonBackground gets destroyed. It stops the script engine, which in
  // turn releases all references to this object.
  void UnInit();

  HRESULT GetLogWindow(CLogWindowComObject * & pLogWindow);
  HRESULT GetContentAPI(ULONG ulInstanceID, LPDISPATCH* ppDisp);
  HRESULT ReleaseContentAPI(ULONG ulInstanceID);

  BOOL GetURL(/*in, out*/ CStringW & sURL);

  void OnAddonServiceReleased();

public:
  // -------------------------------------------------------------------------
  // IAnchoBackgroundAPI methods. See .idl for description.
  STDMETHOD(get_id)(BSTR * pVal);
  STDMETHOD(get_guid)(BSTR * pVal);
  STDMETHOD(startBackgroundWindow)(BSTR bsPartialURL);

  STDMETHOD(addEventObject)(BSTR aEventName, INT aInstanceId, LPDISPATCH aListener);
  STDMETHOD(removeEventObject)(BSTR aEventName, INT aInstanceId);
  STDMETHOD(invokeEventObject)(BSTR aEventName, INT aSelectedInstance, BOOL aSkipInstance, LPDISPATCH aArgs, VARIANT* aRet);
  STDMETHOD(invokeEventWithIDispatchArgument)(BSTR aEventName, LPDISPATCH aArg);
  STDMETHOD(setIDispatchEventInvocationHandler)(LPDISPATCH aFunction);
  STDMETHOD(callFunction)(LPDISPATCH aFunction, LPDISPATCH aArgs, VARIANT* pvRet);


  // -------------------------------------------------------------------------
  // _IMagpieLoggerEvents methods
  STDMETHOD_(void, OnLog)(BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD_(void, OnDebug)(BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD_(void, OnInfo)(BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD_(void, OnWarn)(BSTR bsModuleID, SAFEARRAY* pVals);
  STDMETHOD_(void, OnError)(BSTR bsModuleID, SAFEARRAY* pVals);

  struct EventObjectRecord
  {
    EventObjectRecord(const std::wstring &aEventName, INT aInstanceId, LPDISPATCH aListener)
      : eventName(aEventName), instanceID(aInstanceId), listener(aListener)
    {}
    std::wstring eventName;
    int instanceID;
    CIDispatchHelper listener;
  };
  typedef std::list<EventObjectRecord> EventObjectList;
  typedef std::map<std::wstring, EventObjectList> EventObjectMap;

private:
  // -------------------------------------------------------------------------
  // Private member functions
  STDMETHOD(invokeEvent)(BSTR aEventName, INT aSelectedInstance, bool aSkipInstance, VariantVector &aArgs, VariantVector &aResults);

  HRESULT GetMainModuleExportsScript(CIDispatchHelper & script);

private:
  // -------------------------------------------------------------------------
  // Private variables
#ifndef MAGPIE_REGISTERED
  static _ATL_FUNC_INFO info_Console_LogFunction;
#endif
  static LPCTSTR      s_BackgroundLogIdentifyer;

  // My id
  CComBSTR            m_bsID;

  // My generated extension ID
  CString             m_GUID;

  CStringW            m_sRootURL;

  // magpie
  CComPtr<IMagpieApplication>   m_Magpie;

  // advise cookie for magpie console logging
  DWORD                         m_dwMagpieSinkCookie;

  CComPtr<CLogWindowComObject>
                                m_LogWindow;

  CComPtr<CBackgroundWindowComObject>
                                m_BackgroundWindow;

  EventObjectMap                m_EventObjects;

  CComPtr<IAnchoServiceApi>     m_ServiceApi;

  CIDispatchHelper              m_InvokeEventWithIDispatch;
};

