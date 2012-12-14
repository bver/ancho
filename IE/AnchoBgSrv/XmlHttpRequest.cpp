#include "stdafx.h"
#include "XmlHttpRequest.h"

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

STDMETHODIMP CAnchoXmlHttpRequest::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
  ENSURE_RETVAL(pid);
  if (std::wstring(L"prototype") == bstrName) {
    *pid = DISPID_PROTOTYPE;
    return S_OK;
  } else {
    /*LPOLESTR names[] = {(LPOLESTR)bstrName};
    DISPID dispId[1];
    GetIDsOfNames(IID_NULL, names, 1, LANG_NEUTRAL, dispId);
    *pid = dispId[0];
    return S_OK;*/
    return GetIDsOfNames(IID_NULL, (LPOLESTR*)&bstrName, 1, LANG_NEUTRAL, pid);
  }
}

STDMETHODIMP CAnchoXmlHttpRequest::InvokeEx(
                          DISPID id,
                          LCID lcid,
                          WORD wFlags,
                          DISPPARAMS *pdp,
                          VARIANT *pvarRes,
                          EXCEPINFO *pei,
                          IServiceProvider *pspCaller)
{
  if (id == DISPID_PROTOTYPE && (wFlags & DISPATCH_PROPERTYGET)) {
    CComVariant vt(static_cast<IDispatchEx*>(this));
    vt.Detach(pvarRes);
    return S_OK;
  }
  if (id == 0/* && (wFlags & DISPATCH_CONSTRUCT)*/) {
    return create(pvarRes);
  }
  //Fallback to IDipatch Invoke
  return Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, NULL);

  /*switch(wFlags) {
  case DISPATCH_PROPERTYPUT:
  case DISPATCH_PROPERTYPUTREF:
  case DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF:
  case DISPATCH_METHOD:
    return Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, NULL);
    break;
  case DISPATCH_PROPERTYGET:
  case DISPATCH_METHOD|DISPATCH_PROPERTYGET:
    {
      if (id == DISPID_PROTOTYPE) {
        CComVariant vt(static_cast<IDispatchEx*>(this));
        vt.Detach(pvarRes);
      } else {
        return Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, NULL);
      }
      break;
    }
  case DISPATCH_CONSTRUCT:
    IF_FAILED_RET(create(pvarRes));
    break;
  default:
    return E_INVALIDARG;
  }
  return S_OK;*/
}


STDMETHODIMP CAnchoXmlHttpRequest::create(VARIANT *aRequest)
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
    hr = addCustomInternetSecurity(pRequest);
    if (FAILED(hr)) {
      // Succeeded even if we fail to add the custom security object.
      hr = S_FALSE;
    }
  }
  return S_OK;
}

STDMETHODIMP CAnchoXmlHttpRequest::addCustomInternetSecurity(CComPtr<IXMLHttpRequest> pRequest)
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