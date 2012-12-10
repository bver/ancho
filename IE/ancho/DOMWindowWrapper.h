/****************************************************************************
 * DOMWindowWrapper.h : Declaration of the DOMWindowWrapper
 * Copyright 2012 Salsita Software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once

/*===========================================================================
 * class DOMWindowWrapper
 *  Wraps a DOM window for the content scripts.
 *  This is kind of a sandbox, originally created to prevent jquery from
 *  messing around with content windows.
 */
class ATL_NO_VTABLE DOMWindowWrapper :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchEx
{
public:
  typedef CComObject<DOMWindowWrapper>  ComObject;

  // -------------------------------------------------------------------------
  // static methods
  static HRESULT createInstance(IWebBrowser2 * aWebBrowser,
                                CComPtr<ComObject> & aRet);

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(DOMWindowWrapper)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IDispatchEx)
  END_COM_MAP()

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  void FinalRelease();

  // helper - see comment in implementation
  void forceDelete();

public:
  // -------------------------------------------------------------------------
  // IDispatch methods: simply forward
  __forceinline STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
    { return mDOMWindow->GetTypeInfoCount(pctinfo); }
  __forceinline STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
    { return mDOMWindow->GetTypeInfo(iTInfo, lcid, ppTInfo); }
  __forceinline STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames,
                                         LCID lcid, DISPID *rgDispId)
    { return mDOMWindow->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId); }
  __forceinline STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                                  DISPPARAMS *pDispParams, VARIANT *pVarResult,
                                  EXCEPINFO *pExcepInfo, UINT *puArgErr)
    { return mDOMWindow->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams,
                                pVarResult, pExcepInfo, puArgErr); }

  // -------------------------------------------------------------------------
  // IDispatchEx methods
  STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid);
  STDMETHOD(InvokeEx)(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp,
                      VARIANT *pvarRes, EXCEPINFO *pei,
                      IServiceProvider *pspCaller);
  STDMETHOD(DeleteMemberByName)(BSTR bstrName,DWORD grfdex);
  STDMETHOD(DeleteMemberByDispID)(DISPID id);
  STDMETHOD(GetMemberProperties)(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex);
  STDMETHOD(GetMemberName)(DISPID id, BSTR *pbstrName);
  STDMETHOD(GetNextDispID)(DWORD grfdex, DISPID id, DISPID *pid);
  STDMETHOD(GetNameSpaceParent)(IUnknown **ppunk);

private:
  // -------------------------------------------------------------------------
  // ctor
  DOMWindowWrapper()
    {}

  HRESULT init(IWebBrowser2 * aWebBrowser);

  // Map DISPIDs of on... properties to their names. The on... properties need
  // a special handling (see dispatchPropertyPut).
  static const wchar_t *getEventPropertyName(DISPID id);

  // -------------------------------------------------------------------------
  // IDispatchEx::InvokeEx handlers.
  HRESULT dispatchMethod(DISPID dispIdMember, REFIID riid, LCID lcid,
                    DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, IServiceProvider *pspCaller,
                    BOOL & aHandled) const;
  HRESULT dispatchPropertyGet(WORD wFlags, DISPID dispIdMember, REFIID riid,
                    LCID lcid, DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, IServiceProvider *pspCaller,
                    BOOL & aHandled) const;
  HRESULT dispatchPropertyPut(WORD wFlags, DISPID dispIdMember, REFIID riid,
                    LCID lcid, DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, IServiceProvider *pspCaller,
                    BOOL & aHandled);
  HRESULT dispatchConstruct(DISPID dispIdMember, REFIID riid,
                    LCID lcid, DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, IServiceProvider *pspCaller,
                    BOOL & aHandled) const;

private:
  // -------------------------------------------------------------------------
  // Private types.
  typedef std::map<DISPID, CComVariant> MapDISPIDToCComVariant;
  typedef std::map<CComBSTR, DISPID> MapNameToDISPID;
  typedef std::map<DISPID, CComBSTR> MapDISPIDToName;

  // -------------------------------------------------------------------------
  // Private members.

  friend ComObject;   // needs to call constructor

  // The first DISPID for dynamic properties the IE dom window uses.
  // NOTE: This value comes from debugging, it is not documented.
  enum { DISPID_DOMWINDOW_EX_FIRST = 10000 };

  // These DISPIDs are related to on... properties. We have to handle them
  // in a special way, so we need the IDs.
  enum {
    DISPID_ONBEFOREUNLOAD = -2147412073,
    DISPID_ONMESSAGE      = -2147412002,
    DISPID_ONBLUR         = -2147412097,
    DISPID_ONUNLOAD       = -2147412079,
    DISPID_ONHASHCHANGE   = -2147412003,
    DISPID_ONLOAD         = -2147412080,
    DISPID_ONSCROLL       = -2147412081,
    DISPID_ONAFTERPRINT   = -2147412045,
    DISPID_ONRESIZE       = -2147412076,
    DISPID_ONERROR        = -2147412083,
    DISPID_ONHELP         = -2147412099,
    DISPID_ONBEFOREPRINT  = -2147412046,
    DISPID_ONFOCUS        = -2147412098
  };

  // the original DOM window
  CComQIPtr<IDispatchEx>  mDOMWindow;
  // map DISPID -> VARIANT (the actual property)
  MapDISPIDToCComVariant  mDOMWindowProperties;
  // map name -> DISPID
  MapNameToDISPID         mDOMWindowPropertyIDs;
  // map DISPID -> name (for reverse lookups in GetMemberName()
  MapDISPIDToName         mDOMWindowPropertyNames;

  // Event properties. We can't store them in the expando property map
  // because this will mess up GetNextDispID.
  MapDISPIDToCComVariant  mDOMEventProperties;
};
