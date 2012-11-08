/****************************************************************************
 * AnchoBrowserEvents.cpp : Implementation of CAnchoBrowserEvents
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Matthew Gertner <matthew@salsitasoft.com>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoBrowserEvents.h"

/*============================================================================
 * class CAnchoBrowserEvents
 */

//----------------------------------------------------------------------------
//  OnFrameStart
STDMETHODIMP CAnchoBrowserEvents::OnFrameStart(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame)
{
  VARIANTARG* pvars = new VARIANTARG[2];
  pvars[1].vt = VT_BSTR;
  pvars[1].bstrVal= bstrUrl;
  pvars[0].vt = VT_BOOL;
  pvars[0].boolVal= bIsMainFrame ? VARIANT_TRUE : VARIANT_FALSE;
  return FireEvent(1, pvars, 2);
}

//----------------------------------------------------------------------------
//  OnFrameEnd
STDMETHODIMP CAnchoBrowserEvents::OnFrameEnd(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame)
{
  VARIANTARG* pvars = new VARIANTARG[2];
  pvars[1].vt = VT_BSTR;
  pvars[1].bstrVal= bstrUrl;
  pvars[0].vt = VT_BOOL;
  pvars[0].boolVal= bIsMainFrame ? VARIANT_TRUE : VARIANT_FALSE;
  return FireEvent(2, pvars, 2);
}

//----------------------------------------------------------------------------
//  OnFrameRedirect
STDMETHODIMP CAnchoBrowserEvents::OnFrameRedirect(BSTR bstrOldUrl, BSTR bstrNewUrl)
{
  VARIANTARG* pvars = new VARIANTARG[2];
  pvars[1].vt = VT_BSTR;
  pvars[1].bstrVal = bstrOldUrl;
  pvars[0].vt = VT_BSTR;
  pvars[0].bstrVal = bstrNewUrl;
  return FireEvent(3, pvars, 2);
}

//----------------------------------------------------------------------------
//  FireDocumentEvent
HRESULT CAnchoBrowserEvents::FireEvent(DISPID dispid, VARIANTARG* pvars, unsigned int count)
{
  int nConnectionIndex;
  int nConnections = m_vec.GetSize();

  HRESULT hr = S_OK;
  Lock();
  for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
  {
    CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
    if (sp != NULL) {
      DISPPARAMS disp = { pvars, NULL, count, 0 };
      CComQIPtr<IDispatch> pDispatch(sp);
      ATLASSERT(pDispatch != NULL);
      pDispatch->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
    }
  }
  Unlock();
  delete [] pvars;
  return hr;
}