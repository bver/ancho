/****************************************************************************
 * DOMWindowWrapper.cpp : Implementation of DOMWindowWrapper
 * Copyright 2012 Salsita Software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "StdAfx.h"
#include "DOMWindowWrapper.h"

// ---------------------------------------------------------------------------
// HTMLWindowInterfaces::operator =
DOMWindowWrapper::HTMLWindowInterfaces &
    DOMWindowWrapper::HTMLWindowInterfaces::operator = (LPDISPATCH pDisp) {
  dispEx = pDisp;
  w2 = pDisp;
  w3 = pDisp;
  w4 = pDisp;
  w5 = pDisp;
  return *this;
}

// ---------------------------------------------------------------------------
// HTMLWindowInterfaces::getDispID
HRESULT DOMWindowWrapper::HTMLWindowInterfaces::
    getDispID(LPOLESTR name, DISPID & id)
{
  LPOLESTR names[] = {name};
#define RETURN_IDS_OF_NAMES_FROM_IFACE(_if) \
  if (_if && SUCCEEDED(_if->GetIDsOfNames(IID_NULL, names, 1, LANG_NEUTRAL, &id))) { \
    return S_OK; \
  }
  RETURN_IDS_OF_NAMES_FROM_IFACE(w2);
  RETURN_IDS_OF_NAMES_FROM_IFACE(w3);
  RETURN_IDS_OF_NAMES_FROM_IFACE(w4);
  RETURN_IDS_OF_NAMES_FROM_IFACE(w5);
#undef RETURN_IDS_OF_NAMES_FROM_IFACE
  return DISP_E_UNKNOWNNAME;
}

// ---------------------------------------------------------------------------
// HTMLWindowInterfaces::Release
void DOMWindowWrapper::HTMLWindowInterfaces::Release() {
  dispEx.Release();
  w2.Release();
  w3.Release();
  w4.Release();
  w5.Release();
}

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
// getEventPropertyName
const wchar_t *DOMWindowWrapper::getEventPropertyName(DISPID id) {
  // for simplicity and speed we use a hardcoded, static map here
  switch(id) {
    case DISPID_ONBEFOREUNLOAD: return L"onbeforeunload";
    case DISPID_ONMESSAGE: return L"onmessage";
    case DISPID_ONBLUR: return L"onblur";
    case DISPID_ONUNLOAD: return L"onunload";
    case DISPID_ONHASHCHANGE: return L"onhashchange";
    case DISPID_ONLOAD: return L"onload";
    case DISPID_ONSCROLL: return L"onscroll";
    case DISPID_ONAFTERPRINT: return L"onafterprint";
    case DISPID_ONRESIZE: return L"onresize";
    case DISPID_ONERROR: return L"onerror";
    case DISPID_ONHELP: return L"onhelp";
    case DISPID_ONBEFOREPRINT: return L"onbeforeprint";
    case DISPID_ONFOCUS: return L"onfocus";
  }
  return NULL;
}

// ---------------------------------------------------------------------------
// init
HRESULT DOMWindowWrapper::init(IWebBrowser2 * aWebBrowser)
{
  if (!aWebBrowser) {
    return E_INVALIDARG;
  }
  mDOMWindowInterfaces = CIDispatchHelper::GetScriptDispatch(aWebBrowser);
  if (!mDOMWindowInterfaces.dispEx) {
    return E_INVALIDARG;
  }

  // also create NULL properties for all events in getEventPropertyName
  mDOMEventProperties[DISPID_ONBEFOREUNLOAD].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONMESSAGE].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONBLUR].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONUNLOAD].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONHASHCHANGE].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONLOAD].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONSCROLL].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONAFTERPRINT].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONRESIZE].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONERROR].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONHELP].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONBEFOREPRINT].vt = VT_NULL;
  mDOMEventProperties[DISPID_ONFOCUS].vt = VT_NULL;

  return S_OK;
}

// -------------------------------------------------------------------------
// forceDelete
void DOMWindowWrapper::cleanup()
{
  // *sigh* - jquery (and probably other scripts) leaks like Deepwater
  // Horizon on IE, so we don't get released. Let's at least release
  // everything we use in this place.
  mDOMWindowInterfaces.Release();
  mDOMWindowProperties.clear();
  mDOMWindowPropertyIDs.clear();
  mDOMWindowPropertyNames.clear();
  mDOMEventProperties.clear();
}

// ---------------------------------------------------------------------------
// FinalRelease
void DOMWindowWrapper::FinalRelease()
{
  cleanup();
}

// ---------------------------------------------------------------------------
// dispatchMethod
HRESULT DOMWindowWrapper::dispatchMethod(DISPID dispIdMember, REFIID riid,
                                         LCID lcid, DISPPARAMS *pDispParams,
                                         VARIANT *pVarResult,
                                         EXCEPINFO *pExcepInfo,
                                         IServiceProvider *pspCaller,
                                         BOOL & aHandled) const
{
  if (dispIdMember < DISPID_DOMWINDOW_EX_FIRST) {
    return S_OK;  // original window will handle
  }

  aHandled = TRUE;
  // we handle it
  MapDISPIDToCComVariant::const_iterator it
    = mDOMWindowProperties.find(dispIdMember);
  if (it != mDOMWindowProperties.end()) {
    if (it->second.vt != VT_DISPATCH) {
      return DISP_E_TYPEMISMATCH;
    }
    return it->second.pdispVal->Invoke(0, riid, lcid, DISPATCH_METHOD,
        pDispParams, pVarResult, pExcepInfo, NULL);
  }


  return DISP_E_MEMBERNOTFOUND;
}

// ---------------------------------------------------------------------------
// dispatchPropertyGet
HRESULT DOMWindowWrapper::dispatchPropertyGet(WORD wFlags, DISPID dispIdMember,
                                         REFIID riid,
                                         LCID lcid, DISPPARAMS *pDispParams,
                                         VARIANT *pVarResult,
                                         EXCEPINFO *pExcepInfo,
                                         IServiceProvider *pspCaller,
                                         BOOL & aHandled) const
{
  MapDISPIDToCComVariant::const_iterator it = mDOMWindowProperties.end();
  BOOL propertyFound = FALSE;

  const wchar_t * eventName = getEventPropertyName(dispIdMember);
  if (eventName) {
    // if an event property is requested...
    it = mDOMEventProperties.find(dispIdMember);
    propertyFound = (it != mDOMEventProperties.end());
  }
  else {
    it = mDOMWindowProperties.find(dispIdMember);
    propertyFound = (it != mDOMWindowProperties.end());

    if (!propertyFound && dispIdMember < DISPID_DOMWINDOW_EX_FIRST) {
      // no event, stored property or an expando property: let original window handle it
      return S_OK;
    }
  }

  aHandled = TRUE;
  // we handle it
  if (propertyFound) {
    ATLASSERT(pVarResult);
    ::VariantCopy(pVarResult, &it->second);
    return S_OK;
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
                                         BOOL & aHandled)
{
  // The on.... properties store some event handlers that will not get called
  // if set on the wrapper. To work around this problem we use attachEvent
  // instead.
  const wchar_t * eventName = getEventPropertyName(dispIdMember);
  if (eventName) {
    // Yes, it is an event. Prepare arguments for detachEvent / attachEvent.
    CComVariant dispparamsVariants[] = {NULL, eventName};
    DISPPARAMS params = {dispparamsVariants, NULL, 2, 0};

    CIDispatchHelper script(mDOMWindowInterfaces.dispEx);

    // if we have a handler stored for this event remove the old event handler
    // first
    MapDISPIDToCComVariant::const_iterator it =
        mDOMEventProperties.find(dispIdMember);
    if (it != mDOMEventProperties.end() && VT_NULL != it->second.vt) {
      dispparamsVariants[0] = it->second;
      script.Call(L"detachEvent", &params);
    }

    // now attach new handler
    if (VT_DISPATCH == pDispParams->rgvarg[0].vt) {
      dispparamsVariants[0] = pDispParams->rgvarg[0];
      script.Call(L"attachEvent", &params);
    }
    // store the property
    mDOMEventProperties[dispIdMember] = pDispParams->rgvarg[0];
  }
  else {
    // store the property
    mDOMWindowProperties[dispIdMember] = pDispParams->rgvarg[0];
  }
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
                                         BOOL & aHandled) const
{
  // do nothing, window will handle this
  return E_NOTIMPL;
}

// ---------------------------------------------------------------------------
// IDispatchEx methods

// ---------------------------------------------------------------------------
// GetDispID
STDMETHODIMP DOMWindowWrapper::GetDispID(BSTR bstrName, DWORD grfdex,
                                         DISPID *pid)
{
  if (!pid) {
    return E_POINTER;
  }

  // check if we already have this DISPID in our local map
  MapNameToDISPID::const_iterator it = mDOMWindowPropertyIDs.find(bstrName);
  if (it != mDOMWindowPropertyIDs.end()) {
    // yep!
    (*pid) = it->second;
    return S_OK;
  }

  // Don't have it, see if window knows the property and if it is a
  // non-expando property. Unset fdexNameEnsure because the window should
  // not create a property.

  // Check if window has a IHTMLWindow2,3.. property of this name. If so,
  // return that ID, so that things like document, navigator etc are still
  // callable, even if they are mapped to expando properties.
  // Basically this means: Every property that exists in IHTMLWindowN,
  // we map back to its original.
  HRESULT hr = mDOMWindowInterfaces.getDispID(bstrName, *pid);
  if (S_OK == hr && (*pid) < DISPID_DOMWINDOW_EX_FIRST) {
    // yep!
    return hr;
  }
  if (!(grfdex & fdexNameEnsure)) {
    // should not create the property, so just return error
    return DISP_E_UNKNOWNNAME;
  }

  // the property should be created
  hr = mDOMWindowInterfaces.dispEx->GetDispID(bstrName, fdexNameCaseSensitive | fdexNameEnsure, pid);
  if (FAILED(hr)) {
    return hr;
  }
  mDOMWindowPropertyIDs[bstrName] = *pid;
  mDOMWindowPropertyNames[*pid] = bstrName;
  mDOMWindowProperties[*pid].vt = VT_EMPTY;
  return S_OK;
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
          handled);
      break;
    case DISPATCH_PROPERTYGET:
      hrRet = dispatchPropertyGet(wFlags, id, IID_NULL, lcid, pdp, pvarRes,
          pei, pspCaller, handled);
      break;
    case DISPATCH_METHOD|DISPATCH_PROPERTYGET:
      // http://msdn.microsoft.com/en-us/library/windows/desktop/ms221486%28v=vs.85%29.aspx :
      // "Some languages cannot distinguish between retrieving a property and
      // calling a method. In this case, you should set the flags
      // DISPATCH_PROPERTYGET and DISPATCH_METHOD".
      // This means, we have to handle both cases depending on the actual type.
      // If it is callable call it, otherwise return the property
      hrRet = dispatchMethod(id, IID_NULL, lcid, pdp, pvarRes, pei, pspCaller,
          handled);
      if (DISP_E_TYPEMISMATCH == hrRet) {
        hrRet = dispatchPropertyGet(wFlags, id, IID_NULL, lcid, pdp, pvarRes,
            pei, pspCaller, handled);
      }
      break;
    case DISPATCH_PROPERTYPUT:
    case DISPATCH_PROPERTYPUTREF:
    case DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF:
      hrRet = dispatchPropertyPut(wFlags, id, IID_NULL, lcid, pdp, pvarRes,
          pei, pspCaller, handled);
      break;
    case DISPATCH_CONSTRUCT:
      hrRet = dispatchConstruct(id, IID_NULL, lcid, pdp, pvarRes, pei,
          pspCaller, handled);
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

  return mDOMWindowInterfaces.dispEx->InvokeEx(id, lcid, wFlags, pdp, pvarRes, pei, pspCaller);
}

// ---------------------------------------------------------------------------
// DeleteMemberByName
STDMETHODIMP DOMWindowWrapper::DeleteMemberByName(BSTR bstrName,DWORD grfdex)
{
  // Wow, who would have guessed - IE does not support deleting properties on
  // window!
  // This means in turn: We don't have to do anything here, just let window
  // return an error.
  return mDOMWindowInterfaces.dispEx->DeleteMemberByName(bstrName, grfdex);
}

// ---------------------------------------------------------------------------
// DeleteMemberByDispID
STDMETHODIMP DOMWindowWrapper::DeleteMemberByDispID(DISPID id)
{
  // see DeleteMemberByName
  return mDOMWindowInterfaces.dispEx->DeleteMemberByDispID(id);
}

// ---------------------------------------------------------------------------
// GetMemberProperties
STDMETHODIMP DOMWindowWrapper::GetMemberProperties(DISPID id, DWORD
                                                   grfdexFetch, DWORD *pgrfdex)
{
  // for now it looks like we don't need this
  ATLASSERT(0 && "Seems we have to implement this");
  return mDOMWindowInterfaces.dispEx->GetMemberProperties(id, grfdexFetch, pgrfdex);
}

// ---------------------------------------------------------------------------
// GetMemberName
STDMETHODIMP DOMWindowWrapper::GetMemberName(DISPID id, BSTR *pbstrName)
{
  HRESULT hr = mDOMWindowInterfaces.dispEx->GetMemberName(id, pbstrName);
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
  HRESULT hr = E_FAIL;
  MapDISPIDToCComVariant::const_iterator it = mDOMWindowProperties.end();
  if (id < DISPID_DOMWINDOW_EX_FIRST) {
    // The previous property is still within the range of non-expando
    // properties, so let's ask the window for the next id
    hr = mDOMWindowInterfaces.dispEx->GetNextDispID(grfdex, id, pid);
    if (S_OK == hr && (*pid) < DISPID_DOMWINDOW_EX_FIRST) {
      // a property is found AND is also in the non-expando range, so we are done
      return hr;
    }
    // Window has no more properties OR the property found is an expando
    // property, so continue with our own properties.
    // The previous id is < DISPID_DOMWINDOW_EX_FIRST, so get our first id.
    if (mDOMWindowProperties.empty()) {
      // well, we don't have any
      return S_FALSE;
    }
    // set iterator to begin
    it = mDOMWindowProperties.begin();
  }
  else {
    // The previous id is >= DISPID_DOMWINDOW_EX_FIRST, so get the next id
    // from our own list of properties.
    it = mDOMWindowProperties.find(id);
    if (it == mDOMWindowProperties.end()) {
      // not found - this should not happen
      ATLASSERT(0);
      return S_FALSE;
    }
    // set iterator to next
    ++it;
  }
  if (it == mDOMWindowProperties.end()) {
    // nothing any more
    return S_FALSE;
  }
  // now we have the next id
  (*pid) = it->first;
  return S_OK;
}

// ---------------------------------------------------------------------------
// GetNameSpaceParent
STDMETHODIMP DOMWindowWrapper::GetNameSpaceParent(IUnknown **ppunk)
{
  return mDOMWindowInterfaces.dispEx->GetNameSpaceParent(ppunk);
}
