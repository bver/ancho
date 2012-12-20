#include "stdafx.h"
#include "XmlHttpRequest.h"

//TODO: Implementation copied from aji - needs cleanup
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
  STDMETHOD(SetSecuritySite)(IInternetSecurityMgrSite *pSite)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(GetSecuritySite)(IInternetSecurityMgrSite **ppSite)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(MapUrlToZone)(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags)
  {
    *pdwZone = URLZONE_TRUSTED;
    return S_OK;
  }

  STDMETHOD(GetSecurityId)(LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(ProcessUrlAction)(LPCWSTR pwszUrl,
                              DWORD dwAction,
                              BYTE *pPolicy,
                              DWORD cbPolicy,
                              BYTE *pContext,
                              DWORD cbContext,
                              DWORD dwFlags,
                              DWORD dwReserved)
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

  STDMETHOD(QueryCustomPolicy)(LPCWSTR pwszUrl,
                               REFGUID guidKey,
                               BYTE **ppPolicy,
                               DWORD *pcbPolicy,
                               BYTE *pContext,
                               DWORD cbContext,
                               DWORD dwReserved)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(SetZoneMapping)(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags)
  { return INET_E_DEFAULT_ACTION; }

  STDMETHOD(GetZoneMappings)(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags)
  { return INET_E_DEFAULT_ACTION; }

  // IOleCommandTarget
public:
  STDMETHOD (QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
  { return E_NOTIMPL; }

  STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANTARG *pvaIn, VARIANTARG *pvaOut)
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

  STDMETHODIMP GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
  { return E_NOTIMPL; }

  STDMETHODIMP GetContainer(IOleContainer **ppContainer)
  { return E_NOTIMPL; }

  STDMETHODIMP ShowObject()
  { return S_OK; }

  STDMETHODIMP OnShowWindow(BOOL fShow)
  { return S_OK; }

  STDMETHODIMP RequestNewObjectLayout()
  { return E_NOTIMPL; }

};

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoXmlHttpRequest::GetDispID(BSTR bstrName, DWORD grfdex, DISPID *pid)
{
  ENSURE_RETVAL(pid);
  if (std::wstring(L"prototype") == bstrName) {
    *pid = DISPID_PROTOTYPE;
    return S_OK;
  } else {
    return GetIDsOfNames(IID_NULL, (LPOLESTR*)&bstrName, 1, LANG_NEUTRAL, pid);
  }
}
//----------------------------------------------------------------------------
//
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
  if (id == 0 && (wFlags & DISPATCH_CONSTRUCT || wFlags == 0)) { //sometimes constructor called with zero flags !?
    return create(pvarRes);
  }
  //Fallback to IDipatch Invoke
  return Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, NULL);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoXmlHttpRequest::create(VARIANT *aRequest)
{
  ENSURE_RETVAL(aRequest);

  HRESULT hr;
  CComPtr<IAnchoXmlHttpRequest> pRequest;
  CAnchoXmlHttpRequestComObject *request;
  CAnchoXmlHttpRequestComObject::CreateInstance(&request);
  pRequest = request;
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

STDMETHODIMP CAnchoXmlHttpRequest::addCustomInternetSecurity(CComPtr<IAnchoXmlHttpRequest> pRequest)
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