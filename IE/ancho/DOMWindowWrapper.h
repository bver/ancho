/****************************************************************************
 * DOMWindowWrapper.h : Declaration of the DOMWindowWrapper
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
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
  // IDispatch methods
  STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
  STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
  STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames,
                           LCID lcid, DISPID *rgDispId);
  STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                    DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, UINT *puArgErr);

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
  {
  }

  HRESULT init(IWebBrowser2 * aWebBrowser);

  static const wchar_t *getEventHandlerPropertyName(DISPID id) {
    // for simplicity and speed we use a hardcoded, static map here
    switch(id) {
      case -2147412073: return L"onbeforeunload";
      case -2147412002: return L"onmessage";
      case -2147412097: return L"onblur";
      case -2147412079: return L"onunload";
      case -2147412003: return L"onhashchange";
      case -2147412080: return L"onload";
      case -2147412081: return L"onscroll";
      case -2147412045: return L"onafterprint";
      case -2147412076: return L"onresize";
      case -2147412083: return L"onerror";
      case -2147412099: return L"onhelp";
      case -2147412046: return L"onbeforeprint";
      case -2147412098: return L"onfocus";
      case -2147417605: return L"attachEvent";
    }
    return NULL;
  }

  // -------------------------------------------------------------------------
  // IDispatch::Invoke and IDispatchEx::InvokeEx handlers
  // For calls from Invoke pspCaller is NULL, for InvokeEx puArgErr is NULL.
  // riid is IID_NULL for InvokeEx.
  HRESULT dispatchMethod(DISPID dispIdMember, REFIID riid, LCID lcid,
                    DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, IServiceProvider *pspCaller,
                    UINT *puArgErr, BOOL aIsInvokeEx, BOOL & aHandled) const;
  HRESULT dispatchPropertyGet(WORD wFlags, DISPID dispIdMember, REFIID riid,
                    LCID lcid, DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, IServiceProvider *pspCaller,
                    UINT *puArgErr, BOOL aIsInvokeEx, BOOL & aHandled) const;
  HRESULT dispatchPropertyPut(WORD wFlags, DISPID dispIdMember, REFIID riid,
                    LCID lcid, DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, IServiceProvider *pspCaller,
                    UINT *puArgErr, BOOL aIsInvokeEx, BOOL & aHandled);
  HRESULT dispatchConstruct(DISPID dispIdMember, REFIID riid,
                    LCID lcid, DISPPARAMS *pDispParams, VARIANT *pVarResult,
                    EXCEPINFO *pExcepInfo, IServiceProvider *pspCaller,
                    UINT *puArgErr, BOOL & aHandled) const;

private:
  typedef std::map<DISPID, CComVariant> MapDISPIDToCComVariant;
  typedef std::map<CComBSTR, DISPID> MapNameToDISPID;
  typedef std::map<DISPID, CComBSTR> MapDISPIDToName;

  friend ComObject;   // needs to call constructor

  // -------------------------------------------------------------------------
  // Private members.

  // the original DOM window
  CComQIPtr<IDispatchEx>  mDOMWindow;
  // map DISPID -> VARIANT (the actual property)
  MapDISPIDToCComVariant  mDOMWindowProperties;
  // map name -> DISPID
  MapNameToDISPID         mDOMWindowPropertyIDs;
  // map DISPID -> name (for reverse lookups in GetMemberName()
  MapDISPIDToName         mDOMWindowPropertyNames;
};
