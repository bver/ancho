#pragma once
#include "AnchoBgSrv_i.h"
#include <MsXml.h>
#include <string>

class CAnchoXmlHttpRequest;
typedef CComObject<CAnchoXmlHttpRequest> CAnchoXmlHttpRequestComObject;

class ATL_NO_VTABLE CAnchoXmlHttpRequest :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CAnchoXmlHttpRequest, &CLSID_AnchoXmlHttpRequest>,
  public IObjectWithSite,
  public IDispatchImpl<IAnchoXmlHttpRequest, &IID_IAnchoXmlHttpRequest, &LIBID_AnchoBgSrvLib, 0xffff, 0xffff>,
  public IDispatchEx
{
public:
  typedef IDispatchImpl<IAnchoXmlHttpRequest, &IID_IAnchoXmlHttpRequest, &LIBID_AnchoBgSrvLib, 0xffff, 0xffff> IDispatchImplPredecessor;

  static const DISPID DISPID_PROTOTYPE = 8000;
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
    return IDispatchImplPredecessor::GetTypeInfoCount(pctinfo);
  }

  STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
  {
    return IDispatchImplPredecessor::GetTypeInfo(iTInfo, lcid, ppTInfo);
  }

  STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
  {
    return IDispatchImplPredecessor::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
  }

  STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                                  DISPPARAMS *pDispParams, VARIANT *pVarResult,
                                  EXCEPINFO *pExcepInfo, UINT *puArgErr)
  {
    return IDispatchImplPredecessor::Invoke(dispIdMember, riid, lcid, wFlags,
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
  {
    HRESULT hr = mRequest->get_status(plStatus);
    if (hr == S_OK && *plStatus == 0) { //Workaround - local file request returns 0
      *plStatus = 200;
    }
    return hr;
  }

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


  // -------------------------------------------------------------------------
  // IDispatchEx methods
  STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid);

  STDMETHOD(InvokeEx)(DISPID id,
                      LCID lcid,
                      WORD wFlags,
                      DISPPARAMS *pdp,
                      VARIANT *pvarRes,
                      EXCEPINFO *pei,
                      IServiceProvider *pspCaller);

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

protected:
  STDMETHOD(create)(VARIANT *aRequest);

  STDMETHOD(addCustomInternetSecurity)(CComPtr<IAnchoXmlHttpRequest> pRequest);

  CComPtr<IXMLHttpRequest> mRequest;
};

OBJECT_ENTRY_AUTO(__uuidof(AnchoXmlHttpRequest), CAnchoXmlHttpRequest)
