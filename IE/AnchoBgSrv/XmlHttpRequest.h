#pragma once
#include "AnchoBgSrv_i.h"
#include <MsXml.h>
#include <string>

class CAnchoXmlHttpRequest;
typedef CComObject<CAnchoXmlHttpRequest> CAnchoXmlHttpRequestComObject;

class ATL_NO_VTABLE CAnchoXmlHttpRequest :
	public CComObjectRootEx<CComSingleThreadModel>,
  public IObjectWithSite,
	public IDispatchImpl<IAnchoXmlHttpRequest, &IID_IAnchoXmlHttpRequest, &LIBID_AnchoBgSrvLib,
                      /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>,
  public IDispatchEx
{
public:
  typedef IDispatchImpl<IAnchoXmlHttpRequest, &IID_IAnchoXmlHttpRequest, &LIBID_AnchoBgSrvLib,
                      /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff> IDispatchImlPredecessor;
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CAnchoXmlHttpRequest)
	  COM_INTERFACE_ENTRY(IAnchoXmlHttpRequest)
	  COM_INTERFACE_ENTRY2(IDispatch, IAnchoXmlHttpRequest)
    COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY(IXMLHttpRequest)
    COM_INTERFACE_ENTRY(IDispatchEx)
  END_COM_MAP()


public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct()
  {
    return mRequest.CoCreateInstance(__uuidof(XMLHTTPRequest));
  }

	void FinalRelease()
  {}

  STDMETHOD(SetSite)(IUnknown *pUnkSite)
  {
    CComPtr<IObjectWithSite> pObjWithSite;
    IF_FAILED_RET(mRequest->QueryInterface(&pObjWithSite));
    return pObjWithSite->SetSite(pUnkSite);
  }

  STDMETHOD(GetSite)(REFIID riid, void **ppvSite)
  {
    CComPtr<IObjectWithSite> pObjWithSite;
    IF_FAILED_RET(mRequest->QueryInterface(&pObjWithSite));
    return pObjWithSite->GetSite(riid, ppvSite);
  }
public:
  // -------------------------------------------------------------------------
  // public members
  STDMETHOD(get_withCredentials)(VARIANT *aRet)
  {
    ENSURE_RETVAL(aRet);

    aRet->vt = VT_BOOL;
    aRet->boolVal = VARIANT_FALSE;
    return S_OK;
  }

    // -------------------------------------------------------------------------
  // IDispatch methods: simply forward
  STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
  {
    return IDispatchImlPredecessor::GetTypeInfoCount(pctinfo);
  }

  STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
  {
    return IDispatchImlPredecessor::GetTypeInfo(iTInfo, lcid, ppTInfo);
  }

  STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
  {
    return IDispatchImlPredecessor::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
  }

  STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                                  DISPPARAMS *pDispParams, VARIANT *pVarResult,
                                  EXCEPINFO *pExcepInfo, UINT *puArgErr)
  {
    return IDispatchImlPredecessor::Invoke(dispIdMember, riid, lcid, wFlags,
                                  pDispParams, pVarResult, pExcepInfo, puArgErr);
  }

  // -------------------------------------------------------------------------
  // IXMLHttpRequest methods
  HRESULT STDMETHODCALLTYPE open(BSTR bstrMethod, BSTR bstrUrl, VARIANT varAsync, VARIANT bstrUser, VARIANT bstrPassword)
  { return mRequest->open(bstrMethod, bstrUrl, varAsync, bstrUser, bstrPassword); }

  HRESULT STDMETHODCALLTYPE setRequestHeader(BSTR bstrHeader, BSTR bstrValue)
  { return mRequest->setRequestHeader(bstrHeader, bstrValue); }

  HRESULT STDMETHODCALLTYPE getResponseHeader(BSTR bstrHeader, BSTR *pbstrValue)
  { return mRequest->getResponseHeader(bstrHeader, pbstrValue); }

  HRESULT STDMETHODCALLTYPE getAllResponseHeaders(BSTR *pbstrHeaders)
  { return mRequest->getAllResponseHeaders(pbstrHeaders); }

  HRESULT STDMETHODCALLTYPE send(VARIANT varBody)
  { return mRequest->send(varBody); }

  HRESULT STDMETHODCALLTYPE abort()
  { return mRequest->abort(); }

  HRESULT STDMETHODCALLTYPE get_status(long *plStatus)
  { return mRequest->get_status(plStatus); }

  HRESULT STDMETHODCALLTYPE get_statusText(BSTR *pbstrStatus)
  { return mRequest->get_statusText(pbstrStatus); }

  HRESULT STDMETHODCALLTYPE get_responseXML(IDispatch **ppBody)
  { return mRequest->get_responseXML(ppBody); }

  HRESULT STDMETHODCALLTYPE get_responseText(BSTR *pbstrBody)
  { return mRequest->get_responseText(pbstrBody); }

  HRESULT STDMETHODCALLTYPE get_responseBody(VARIANT *pvarBody)
  { return mRequest->get_responseBody(pvarBody); }

  HRESULT STDMETHODCALLTYPE get_readyState(long *plState)
  { return mRequest->get_readyState(plState); }


  HRESULT STDMETHODCALLTYPE get_responseStream(VARIANT *pvarBody)
  { return mRequest->get_responseStream(pvarBody); }

  HRESULT STDMETHODCALLTYPE put_onreadystatechange(IDispatch *pReadyStateSink)
  { return mRequest->put_onreadystatechange(pReadyStateSink); }


  /*HRESULT STDMETHODCALLTYPE put_onreadystatechange(VARIANT v)
  { return mRequest->put_onreadystatechange(v); }

  HRESULT STDMETHODCALLTYPE get_onreadystatechange(VARIANT *p)
  { return mRequest->get_onreadystatechange(p); }*/

  // -------------------------------------------------------------------------
  // IDispatchEx methods
  STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid)
  {
    ENSURE_RETVAL(pid);
    if (std::wstring(L"prototype") == bstrName) {
      *pid = 8000;
      return S_OK;
    } else {
      LPOLESTR names[] = {(LPOLESTR)bstrName};
      DISPID dispId[1];
      GetIDsOfNames(IID_NULL, names, 1, LANG_NEUTRAL, dispId);
      *pid = dispId[0];
      return S_OK;
    }
    return E_NOTIMPL;
  }
  STDMETHOD(InvokeEx)(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp,
                      VARIANT *pvarRes, EXCEPINFO *pei,
                      IServiceProvider *pspCaller)
  {
    switch(wFlags) {
    case DISPATCH_PROPERTYGET:
    case DISPATCH_METHOD|DISPATCH_PROPERTYGET:
      {
        if (id != 8000) {
          return E_INVALIDARG;
        }
        CComVariant vt(static_cast<IDispatchEx*>(this));
        vt.Detach(pvarRes);
        break;
      }
    case DISPATCH_CONSTRUCT:
      IF_FAILED_RET(create(pvarRes));
      break;
    default:
      return E_INVALIDARG;
    }
    return S_OK;
  }
  STDMETHOD(DeleteMemberByName)(BSTR bstrName,DWORD grfdex)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(DeleteMemberByDispID)(DISPID id)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(GetMemberProperties)(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(GetMemberName)(DISPID id, BSTR *pbstrName)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(GetNextDispID)(DWORD grfdex, DISPID id, DISPID *pid)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(GetNameSpaceParent)(IUnknown **ppunk)
  {
    return E_NOTIMPL;
  }

  HRESULT STDMETHODCALLTYPE create(VARIANT *aRequest)
  {
    ENSURE_RETVAL(aRequest);

    HRESULT hr;
    CComPtr<IXMLHttpRequest> pRequest;
    CAnchoXmlHttpRequestComObject *request;
    CAnchoXmlHttpRequestComObject::CreateInstance(&request);
    //IF_FAILED_RET(request->QueryInterface(IID_IDispatch, (void**) ppVal));
    //IF_FAILED_RET(request->QueryInterface(IID_IXMLHttpRequest, (void**)pRequest.p));
    pRequest = request;
    //hr = pRequest.CoCreateInstance(__uuidof(XMLHTTPRequest));
    if (pRequest) {
      aRequest->vt = VT_DISPATCH;
      hr = pRequest->QueryInterface(IID_IAnchoXmlHttpRequest, (void**) &(aRequest->pdispVal));
    } else {
      return E_FAIL;
    }
    if (SUCCEEDED(hr)) {
      hr = AddCustomInternetSecurity(pRequest);
      if (FAILED(hr)) {
        // Succeeded even if we fail to add the custom security object.
        hr = S_FALSE;
      }
    }
    return S_OK;
  }

  HRESULT AddCustomInternetSecurity(CComPtr<IXMLHttpRequest> pRequest)
  {
    /*CComObject<CCustomInternetSecurityImpl>* pSecurityImpl = NULL;
    CComPtr<IUnknown> pUnkSecurity;
    CComPtr<IObjectWithSite> pObjWithSite;

    IF_FAILED_RET(CComObject<CCustomInternetSecurityImpl>::CreateInstance(&pSecurityImpl));
    IF_FAILED_RET(pSecurityImpl->QueryInterface(&pUnkSecurity));
    IF_FAILED_RET(pRequest->QueryInterface(&pObjWithSite));
    IF_FAILED_RET(pObjWithSite->SetSite(pUnkSecurity));*/

    return S_OK;
  }


protected:
  CComPtr<IXMLHttpRequest> mRequest;
};


class ATL_NO_VTABLE CCustomInternetSecurityImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<IDispatch, &IID_IDispatch, NULL, /*wMajor =*/ 1, /*wMinor =*/ 0>,
  public IServiceProviderImpl<CCustomInternetSecurityImpl>,
  public IInternetSecurityManager,
  public IOleCommandTarget,
  public IOleClientSite
{
public:
  CCustomInternetSecurityImpl()
  {
  }

  BEGIN_SERVICE_MAP(CCustomInternetSecurityImpl)
    SERVICE_ENTRY(__uuidof(IInternetSecurityManager))
  END_SERVICE_MAP()

  BEGIN_COM_MAP(CCustomInternetSecurityImpl)
    COM_INTERFACE_ENTRY(IInternetSecurityManager)
    COM_INTERFACE_ENTRY(IServiceProvider)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IOleCommandTarget)
    COM_INTERFACE_ENTRY(IOleClientSite)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
  { return S_OK; }

  void FinalRelease()
  { }

public:
  STDMETHOD(SetSecuritySite)(
    /* [unique][in] */ __RPC__in_opt IInternetSecurityMgrSite *pSite)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(GetSecuritySite)(
    /* [out] */ __RPC__deref_out_opt IInternetSecurityMgrSite **ppSite)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(MapUrlToZone)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [out] */ __RPC__out DWORD *pdwZone,
    /* [in] */ DWORD dwFlags)
  {
    *pdwZone = URLZONE_TRUSTED;
    return S_OK;
  }

  STDMETHOD(GetSecurityId)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [size_is][out] */ __RPC__out_ecount_full(*pcbSecurityId) BYTE *pbSecurityId,
    /* [out][in] */ __RPC__inout DWORD *pcbSecurityId,
    /* [in] */ DWORD_PTR dwReserved)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(ProcessUrlAction)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [in] */ DWORD dwAction,
    /* [size_is][out] */ __RPC__out_ecount_full(cbPolicy) BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ __RPC__in BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwReserved)
  {
    if(pPolicy != NULL)
    {
      // request permissions for java-applet in a banner
      // more info - http://msdn.microsoft.com/library/ms537183.aspx // msdn - About URL Security Zones
       if (URLACTION_JAVA_PERMISSIONS == dwAction)
         *pPolicy = URLPOLICY_QUERY;
       else
        *pPolicy = URLPOLICY_ALLOW;

      return S_OK;
    }
    return INET_E_DEFAULT_ACTION;
  }

  STDMETHOD(QueryCustomPolicy)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [in] */ __RPC__in REFGUID guidKey,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcbPolicy) BYTE **ppPolicy,
    /* [out] */ __RPC__out DWORD *pcbPolicy,
    /* [in] */ __RPC__in BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwReserved)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(SetZoneMapping)(
    /* [in] */ DWORD dwZone,
    /* [in] */ __RPC__in LPCWSTR lpszPattern,
    /* [in] */ DWORD dwFlags)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(GetZoneMappings)(
    /* [in] */ DWORD dwZone,
    /* [out] */ __RPC__deref_out_opt IEnumString **ppenumString,
    /* [in] */ DWORD dwFlags)
  { return INET_E_DEFAULT_ACTION; }

  // IOleCommandTarget
public:
  STDMETHOD (QueryStatus)(
    /* [unique][in] */ const GUID *pguidCmdGroup,
    /* [in] */ ULONG cCmds,
    /* [out][in][size_is] */ OLECMD prgCmds[  ],
    /* [unique][out][in] */ OLECMDTEXT *pCmdText)
  { return E_NOTIMPL; }

  STDMETHOD(Exec)(
    /*[in]*/ const GUID *pguidCmdGroup,
    /*[in]*/ DWORD nCmdID,
    /*[in]*/ DWORD nCmdExecOpt,
    /*[in]*/ VARIANTARG *pvaIn,
    /*[in,out]*/ VARIANTARG *pvaOut)
  {
    HRESULT hr = S_OK;
    if(pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
    {
      switch(nCmdID)
      {
        case OLECMDID_SHOWSCRIPTERROR:
        {
           (*pvaOut).vt = VT_BOOL;
          // Continue running scripts on the page.
           (*pvaOut).boolVal = VARIANT_TRUE;
        }
      }
    }
    return E_NOTIMPL; // TODO: wtf
  }

  //IOleClientSite
  STDMETHODIMP SaveObject()
  { return E_NOTIMPL; }

  STDMETHODIMP GetMoniker(
    /* [in] */ DWORD dwAssign,
    /* [in] */ DWORD dwWhichMoniker,
    /* [out] */ IMoniker **ppmk)
  { return E_NOTIMPL; }

  STDMETHODIMP GetContainer(/* [out] */ IOleContainer **ppContainer)
  { return E_NOTIMPL; }

  STDMETHODIMP ShowObject()
  { return S_OK; }

  STDMETHODIMP OnShowWindow(/* [in] */ BOOL fShow)
  { return S_OK; }

  STDMETHODIMP RequestNewObjectLayout()
  { return E_NOTIMPL; }

  /*STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
  {
    if(IsEqualGUID(IID_IInternetSecurityManager, riid)){
      *ppv=(IInternetSecurityManager*) this;
    }
    else if(IsEqualGUID(IID_IUnknown, riid)){
      *ppv=(IUnknown*)((IOleClientSite*) this);
    }
    else if(IsEqualGUID(IID_IOleClientSite, riid)){
      *ppv=(IOleClientSite*) this;
    }
    else{
      *ppv=NULL;
      return E_NOINTERFACE;
    }

    return S_OK;
  }*/
};

class CAnchoXmlHttpRequestFactory;
typedef CComObject<CAnchoXmlHttpRequestFactory> CAnchoXmlHttpRequestFactoryComObject;

class ATL_NO_VTABLE CAnchoXmlHttpRequestFactory :
	public CComObjectRootEx<CComSingleThreadModel>,
  //public IDispatchImpl<IAnchoXMLHttpRequestFactory, &IID_IAnchoXMLHttpRequestFactory, &LIBID_AnchoBgSrvLib,
  //                    /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>,
  public IDispatchEx
  //public IDispatchImpl<IHTMLXMLHttpRequestFactory, &IID_IHTMLXMLHttpRequestFactory, &LIBID_MSHTML,
  //                    /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
// -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CAnchoXmlHttpRequestFactory)
	  COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IDispatchEx)
  END_COM_MAP()

  // -------------------------------------------------------------------------
  // IDispatch methods: simply forward
  STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
  {
    ENSURE_RETVAL(pctinfo);
    *pctinfo = 0;
    return S_OK;
  }

  STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                                  DISPPARAMS *pDispParams, VARIANT *pVarResult,
                                  EXCEPINFO *pExcepInfo, UINT *puArgErr)
  {
    return E_NOTIMPL;
  }

  // -------------------------------------------------------------------------
  // IDispatchEx methods
  STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid)
  {
    if (std::wstring(L"prototype") == bstrName) {
      *pid = 1;
      return S_OK;
    }
    return E_NOTIMPL;
  }
  STDMETHOD(InvokeEx)(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp,
                      VARIANT *pvarRes, EXCEPINFO *pei,
                      IServiceProvider *pspCaller)
  {
    switch(wFlags) {
    case DISPATCH_PROPERTYGET:
    case DISPATCH_METHOD|DISPATCH_PROPERTYGET:
      {
        if (id != 1) {
          return E_INVALIDARG;
        }
        CComVariant vt(this);
        vt.Detach(pvarRes);
        break;
      }
    case DISPATCH_CONSTRUCT:
      IF_FAILED_RET(create(pvarRes));
      break;
    default:
      return E_INVALIDARG;
    }
    return S_OK;
  }
  STDMETHOD(DeleteMemberByName)(BSTR bstrName,DWORD grfdex)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(DeleteMemberByDispID)(DISPID id)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(GetMemberProperties)(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(GetMemberName)(DISPID id, BSTR *pbstrName)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(GetNextDispID)(DWORD grfdex, DISPID id, DISPID *pid)
  {
    return E_NOTIMPL;
  }
  STDMETHOD(GetNameSpaceParent)(IUnknown **ppunk)
  {
    return E_NOTIMPL;
  }


public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct()
  { return S_OK; }

	void FinalRelease()
  {
    ATLTRACE(L"RELEASING FACTORY----------------------------------\n");
  }

  HRESULT STDMETHODCALLTYPE create(VARIANT *aRequest)
  {
    ENSURE_RETVAL(aRequest);

    HRESULT hr;
    CComPtr<IXMLHttpRequest> pRequest;
    CAnchoXmlHttpRequestComObject *request;
    CAnchoXmlHttpRequestComObject::CreateInstance(&request);
    //IF_FAILED_RET(request->QueryInterface(IID_IDispatch, (void**) ppVal));
    //IF_FAILED_RET(request->QueryInterface(IID_IXMLHttpRequest, (void**)pRequest.p));
    pRequest = request;
    //hr = pRequest.CoCreateInstance(__uuidof(XMLHTTPRequest));
    if (pRequest) {
      aRequest->vt = VT_DISPATCH;
      hr = pRequest->QueryInterface(IID_IDispatch, (void**) &(aRequest->pdispVal));
    } else {
      return E_FAIL;
    }
    if (SUCCEEDED(hr)) {
      hr = AddCustomInternetSecurity(pRequest);
      if (FAILED(hr)) {
        // Succeeded even if we fail to add the custom security object.
        hr = S_FALSE;
      }
    }
    return S_OK;
  }

  HRESULT AddCustomInternetSecurity(CComPtr<IXMLHttpRequest> pRequest)
  {
    CComObject<CCustomInternetSecurityImpl>* pSecurityImpl = NULL;
    CComPtr<IUnknown> pUnkSecurity;
    CComPtr<IObjectWithSite> pObjWithSite;

    IF_FAILED_RET(CComObject<CCustomInternetSecurityImpl>::CreateInstance(&pSecurityImpl));
    IF_FAILED_RET(pSecurityImpl->QueryInterface(&pUnkSecurity));
    IF_FAILED_RET(pRequest->QueryInterface(&pObjWithSite));
    IF_FAILED_RET(pObjWithSite->SetSite(pUnkSecurity));

    return S_OK;
  }

};