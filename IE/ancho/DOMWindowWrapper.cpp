/****************************************************************************
 * DOMWindowWrapper.cpp : Implementation of DOMWindowWrapper
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "StdAfx.h"
#include "DOMWindowWrapper.h"

#define SAFE_FORWARD_CALL(obj, mthd, ...) (obj) ? obj->mthd(__VA_ARGS__) : E_UNEXPECTED

// ---------------------------------------------------------------------------
// createInstance
HRESULT DOMWindowWrapper::createInstance(IWebBrowser2 * aWebBrowser,
                                         CComPtr<ComObject> & aRet)
{
  ComObject * newObject = NULL;
  IF_FAILED_RET(ComObject::CreateInstance(&newObject));
  CComPtr<ComObject> objectRet(newObject);  // to have a refcount of 1
  IF_FAILED_RET(newObject->init(aWebBrowser));
  aRet = objectRet;
  return S_OK;
}

// ---------------------------------------------------------------------------
// init
HRESULT DOMWindowWrapper::init(IWebBrowser2 * aWebBrowser)
{
  if (!aWebBrowser) {
    return E_INVALIDARG;
  }
  mDOMWindow = CIDispatchHelper::GetScriptDispatch(aWebBrowser);
  if (!mDOMWindow) {
    return E_INVALIDARG;
  }
  return S_OK;
}

// -------------------------------------------------------------------------
// forceDelete
void DOMWindowWrapper::forceDelete()
{
  // *sigh* - jquery (and probably other scripts) leaks like Deepwater
  // Horizon on IE, so we have to use dirty tricks to get rid of the
  // fakewindow. NEVER DO THIS NORMALLY!
  // The only reason why it is safe to do this here is that the fakewindow
  // definitely does not get used anymore after magpie is finished.
  // So this means: Call ONLY AFTER MAGPIE IS SHUT DOWN!!

  FinalRelease();
  DWORD outstandingRefs = m_dwRef;
  for (DWORD n = 0; n < outstandingRefs; n++) {
    Release();
  }
}

// ---------------------------------------------------------------------------
// FinalRelease
void DOMWindowWrapper::FinalRelease()
{
  mDOMWindow.Release();
  mDOMWindowProperties.clear();
  mDOMWindowPropertyIDs.clear();
  mDOMWindowPropertyNames.clear();
}

// ---------------------------------------------------------------------------
// dispatchMethod
HRESULT DOMWindowWrapper::dispatchMethod(DISPID dispIdMember, REFIID riid,
                                         LCID lcid, DISPPARAMS *pDispParams,
                                         VARIANT *pVarResult,
                                         EXCEPINFO *pExcepInfo,
                                         IServiceProvider *pspCaller,
                                         UINT *puArgErr, BOOL aIsInvokeEx,
                                         BOOL & aHandled) const
{
  // do nothing, window will handle this
  return E_NOTIMPL;
}

// ---------------------------------------------------------------------------
// dispatchPropertyGet
HRESULT DOMWindowWrapper::dispatchPropertyGet(WORD wFlags, DISPID dispIdMember,
                                         REFIID riid,
                                         LCID lcid, DISPPARAMS *pDispParams,
                                         VARIANT *pVarResult,
                                         EXCEPINFO *pExcepInfo,
                                         IServiceProvider *pspCaller,
                                         UINT *puArgErr, BOOL aIsInvokeEx,
                                         BOOL & aHandled) const
{
  if (!aIsInvokeEx) {
    // goes to original window
    return S_OK;
  }

  MapDISPIDToCComVariant::const_iterator it =
      mDOMWindowProperties.find(dispIdMember);
  if (it != mDOMWindowProperties.end()) {
    aHandled = TRUE;
    ATLASSERT(pVarResult);
    CComVariant vt(it->second);
    return vt.Detach(pVarResult);
  }
  return DISP_E_MEMBERNOTFOUND;
}

// ---------------------------------------------------------------------------
// dispatchPropertyPut
HRESULT DOMWindowWrapper::dispatchPropertyPut(WORD wFlags, DISPID dispIdMember,
                                         REFIID riid,
                                         LCID lcid, DISPPARAMS *pDispParams,
                                         VARIANT *pVarResult,
                                         EXCEPINFO *pExcepInfo,
                                         IServiceProvider *pspCaller,
                                         UINT *puArgErr, BOOL aIsInvokeEx,
                                         BOOL & aHandled)
{
  if (!aIsInvokeEx) {
    // goes to original window
    return S_OK;
  }

  // The on.... properties store some event handlers that will not get called
  // if set on the wrapper. To work around this problem we use attachEvent
  // instead.
  const wchar_t * eventName = getEventHandlerPropertyName(dispIdMember);
  if (eventName) {
    // Yes, it is an event. Prepare arguments.
    CComVariant dispparamsVariants[] = {NULL, eventName};
    DISPPARAMS params = {dispparamsVariants, NULL, 2, 0};

    CIDispatchHelper script(mDOMWindow);

    // if we have a handler stored for this event remove the old event handler
    // first
    MapDISPIDToCComVariant::const_iterator it =
        mDOMWindowProperties.find(dispIdMember);
    if (it != mDOMWindowProperties.end()) {
      dispparamsVariants[0] = it->second;
      script.Call(L"detachEvent", &params);
    }

    // now attach new handler
    if (VT_DISPATCH == pDispParams->rgvarg[0].vt) {
      dispparamsVariants[0] = pDispParams->rgvarg[0];
      script.Call(L"attachEvent", &params);
    }
  }
  // now store the property
  mDOMWindowProperties[dispIdMember] = pDispParams->rgvarg[0];
  aHandled = TRUE;
  return S_OK;
}

// ---------------------------------------------------------------------------
// dispatchConstruct
HRESULT DOMWindowWrapper::dispatchConstruct(DISPID dispIdMember,
                                         REFIID riid,
                                         LCID lcid, DISPPARAMS *pDispParams,
                                         VARIANT *pVarResult,
                                         EXCEPINFO *pExcepInfo,
                                         IServiceProvider *pspCaller,
                                         UINT *puArgErr, BOOL & aHandled) const
{
  // do nothing, window will handle this
  return E_NOTIMPL;
}

// ---------------------------------------------------------------------------
// IDispatch methods

// ---------------------------------------------------------------------------
// GetTypeInfoCount
STDMETHODIMP DOMWindowWrapper::GetTypeInfoCount(UINT *pctinfo)
{
  return SAFE_FORWARD_CALL(mDOMWindow, GetTypeInfoCount, pctinfo);
}

// ---------------------------------------------------------------------------
// GetTypeInfo
STDMETHODIMP DOMWindowWrapper::GetTypeInfo(UINT iTInfo, LCID lcid,
                                           ITypeInfo **ppTInfo)
{
  return SAFE_FORWARD_CALL(mDOMWindow, GetTypeInfo, iTInfo, lcid, ppTInfo);
}

// ---------------------------------------------------------------------------
// GetIDsOfNames
STDMETHODIMP DOMWindowWrapper::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames,
                                             UINT cNames, LCID lcid,
                                             DISPID *rgDispId)
{
  return SAFE_FORWARD_CALL(
      mDOMWindow, GetIDsOfNames, riid, rgszNames, cNames, lcid, rgDispId);
}

// ---------------------------------------------------------------------------
// Invoke
STDMETHODIMP DOMWindowWrapper::Invoke(DISPID dispIdMember, REFIID riid,
                                      LCID lcid, WORD wFlags,
                                      DISPPARAMS *pDispParams,
                                      VARIANT *pVarResult,
                                      EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
  // Note: Invoke will not get called from scripts, everything is aHandled by
  // InvokeEx. However, we have to handle Invoke also
  BOOL handled = FALSE;
  HRESULT hrRet = E_INVALIDARG;
  switch(wFlags) {
    case DISPATCH_METHOD:
      hrRet = dispatchMethod(dispIdMember, riid, lcid, pDispParams, pVarResult,
          pExcepInfo, NULL, puArgErr, FALSE, handled);
      break;
    case DISPATCH_PROPERTYGET:
    case DISPATCH_METHOD|DISPATCH_PROPERTYGET:
      // http://msdn.microsoft.com/en-us/library/windows/desktop/ms221486%28v=vs.85%29.aspx :
      // "Some languages cannot distinguish between retrieving a property and
      // calling a method. In this case, you should set the flags
      // DISPATCH_PROPERTYGET and DISPATCH_METHOD".
      // This means, we have to handle both cases as dispatchPropertyGet.
      hrRet = dispatchPropertyGet(wFlags, dispIdMember, riid, lcid, pDispParams,
          pVarResult, pExcepInfo, NULL, puArgErr, FALSE, handled);
      break;
    case DISPATCH_PROPERTYPUT:
    case DISPATCH_PROPERTYPUTREF:
    case DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF:
      hrRet = dispatchPropertyPut(wFlags, dispIdMember, riid, lcid, pDispParams,
          pVarResult, pExcepInfo, NULL, puArgErr, FALSE, handled);
      break;
    default:
      ATLTRACE(_T("DOMWindowWrapper::Invoke ************* Unknown Flag 0x%08x\n"), wFlags);
      ATLASSERT(0 && "Ooops - did we forget to handle something?");
      return E_INVALIDARG;
      break;
  }
  if (handled) {
    return hrRet;
  }

  return SAFE_FORWARD_CALL(mDOMWindow, Invoke, dispIdMember, riid, lcid,
      wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

// ---------------------------------------------------------------------------
// IDispatchEx methods

// ---------------------------------------------------------------------------
// GetDispID
STDMETHODIMP DOMWindowWrapper::GetDispID(BSTR bstrName, DWORD grfdex,
                                         DISPID *pid)
{
  // check if we already have this DISPID in our local map
  MapNameToDISPID::const_iterator it = mDOMWindowPropertyIDs.find(bstrName);
  if (it != mDOMWindowPropertyIDs.end()) {
    (*pid) = it->second;
    return S_OK;
  }

  // Don't have it yet, let original window do the work. We need a DISPID from
  // the original window, although we don't want to actually set the property
  if (grfdex & fdexNameEnsure)
  {
    // See if the dispid already exists in the original window. To do so call
    // GetDispID without fdexNameEnsure flag.
    HRESULT hr = SAFE_FORWARD_CALL(
        mDOMWindow, GetDispID, bstrName, grfdex & ~fdexNameEnsure, pid);
    if (DISP_E_UNKNOWNNAME == hr)
    {
      // no, don't have, so create it..
      hr = SAFE_FORWARD_CALL(mDOMWindow, GetDispID, bstrName, grfdex, pid);
      if (FAILED(hr))
      {
        return hr;
      }
      // ... and immediately delete it, we don't want to poison the window
      // properties

      // NOTE: See comment in DeleteMemberByName. This call seems to be not
      // required at all, but we keep it - MS likes to change things from time
      // to time, so if one day IE works as it's supposed to we might need it..
      mDOMWindow->DeleteMemberByDispID(*pid);
    }
    else if (FAILED(hr))
    {
      return hr;
    }
    mDOMWindowPropertyIDs[bstrName] = (*pid);
    mDOMWindowPropertyNames[(*pid)] = bstrName;
    return S_OK;
  }
  // all other cases are handled by the original window
  return SAFE_FORWARD_CALL(mDOMWindow, GetDispID, bstrName, grfdex, pid);
}

// ---------------------------------------------------------------------------
// InvokeEx
STDMETHODIMP DOMWindowWrapper::InvokeEx(DISPID id, LCID lcid, WORD wFlags,
                                        DISPPARAMS *pdp, VARIANT *pvarRes,
                                        EXCEPINFO *pei,
                                        IServiceProvider *pspCaller)
{
  BOOL handled = FALSE;
  HRESULT hrRet = E_INVALIDARG;
  switch(wFlags) {
    case DISPATCH_METHOD:
      hrRet = dispatchMethod(id, IID_NULL, lcid, pdp, pvarRes, pei, pspCaller,
          NULL, TRUE, handled);
      break;
    case DISPATCH_PROPERTYGET:
    case DISPATCH_METHOD|DISPATCH_PROPERTYGET:
      // see Invoke
      hrRet = dispatchPropertyGet(wFlags, id, IID_NULL, lcid, pdp, pvarRes,
          pei, pspCaller, NULL, TRUE, handled);
      break;
    case DISPATCH_PROPERTYPUT:
    case DISPATCH_PROPERTYPUTREF:
    case DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF:
      hrRet = dispatchPropertyPut(wFlags, id, IID_NULL, lcid, pdp, pvarRes,
          pei, pspCaller, NULL, TRUE, handled);
      break;
    case DISPATCH_CONSTRUCT:
      hrRet = dispatchConstruct(id, IID_NULL, lcid, pdp, pvarRes, pei,
          pspCaller, NULL, handled);
      break;
    default:
      ATLTRACE(_T("DOMWindowWrapper::InvokeEx ************* Unknown Flag 0x%08x\n"), wFlags);
      ATLASSERT(0 && "Ooops - did we forget to handle something?");
      return E_INVALIDARG;
      break;
  }
  if (handled) {
    return hrRet;
  }

  return SAFE_FORWARD_CALL(mDOMWindow, InvokeEx, id, lcid, wFlags, pdp,
      pvarRes, pei, pspCaller);
}

// ---------------------------------------------------------------------------
// DeleteMemberByName
STDMETHODIMP DOMWindowWrapper::DeleteMemberByName(BSTR bstrName,DWORD grfdex)
{
  // Wow, who would have guessed - IE does not support deleting properties on
  // window!
  // This means in turn: We don't have to do anything here, just let window
  // return an error.
  return SAFE_FORWARD_CALL(mDOMWindow, DeleteMemberByName, bstrName, grfdex);
}

// ---------------------------------------------------------------------------
// DeleteMemberByDispID
STDMETHODIMP DOMWindowWrapper::DeleteMemberByDispID(DISPID id)
{
  // see DeleteMemberByName
  return SAFE_FORWARD_CALL(mDOMWindow, DeleteMemberByDispID, id);
}

// ---------------------------------------------------------------------------
// GetMemberProperties
STDMETHODIMP DOMWindowWrapper::GetMemberProperties(DISPID id, DWORD
                                                   grfdexFetch, DWORD *pgrfdex)
{
  // for now it looks like we don't need this
  ATLASSERT(0 && "Seems we have to implement this");
  return SAFE_FORWARD_CALL(
      mDOMWindow, GetMemberProperties, id, grfdexFetch, pgrfdex);
}

// ---------------------------------------------------------------------------
// GetMemberName
STDMETHODIMP DOMWindowWrapper::GetMemberName(DISPID id, BSTR *pbstrName)
{
  HRESULT hr = SAFE_FORWARD_CALL(mDOMWindow, GetMemberName, id, pbstrName);
  if (S_OK == hr) {
    return hr;
  }
  // hm.. maybe we know the name?
  MapDISPIDToName::const_iterator it = mDOMWindowPropertyNames.find(id);
  if (it != mDOMWindowPropertyNames.end()) {
    it->second.CopyTo(pbstrName);
    return S_OK;
  }
  return hr;
}

// ---------------------------------------------------------------------------
// GetNextDispID
STDMETHODIMP DOMWindowWrapper::GetNextDispID(DWORD grfdex, DISPID id,
                                             DISPID *pid)
{
  // ask window first
  HRESULT hr = SAFE_FORWARD_CALL(mDOMWindow, GetNextDispID, grfdex, id, pid);
  if (S_FALSE == hr) {
    // window has no more properties, continue with our own props now
    if (mDOMWindowProperties.empty()) {
      // well, we don't have any
      return hr;
    }
    // it is important at this point that mDOMWindowProperties is not empty
    MapDISPIDToCComVariant::const_iterator it = mDOMWindowProperties.find(id);
    if (it == mDOMWindowProperties.end()) {
      // id is not in our map, so it should be the last id of the real
      // window, so let's return the first id in our map
      (*pid) = mDOMWindowProperties.begin()->first;
      return S_OK;
    }
    // Ok, it _is_ an id in our map. Now we can find the next id and return it.
    ++it;
    if (it != mDOMWindowProperties.end()) {
      (*pid) = it->first;
      return S_OK;
    }
  }
  return hr;
}

// ---------------------------------------------------------------------------
// GetNameSpaceParent
STDMETHODIMP DOMWindowWrapper::GetNameSpaceParent(IUnknown **ppunk)
{
  return SAFE_FORWARD_CALL(mDOMWindow, GetNameSpaceParent, ppunk);
}
