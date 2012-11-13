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
  CComVariant vt[] = { (VARIANT_BOOL) (bIsMainFrame ? VARIANT_TRUE : VARIANT_FALSE), bstrUrl };
  DISPPARAMS disp = { vt, NULL, 2, 0 };
  return FireEvent(1, &disp, 2);
}

//----------------------------------------------------------------------------
//  OnFrameEnd
STDMETHODIMP CAnchoBrowserEvents::OnFrameEnd(BSTR bstrUrl, VARIANT_BOOL bIsMainFrame)
{
  CComVariant vt[] = { (VARIANT_BOOL) (bIsMainFrame ? VARIANT_TRUE : VARIANT_FALSE), bstrUrl };
  DISPPARAMS disp = { vt, NULL, 2, 0 };
  return FireEvent(2, &disp, 2);
}

//----------------------------------------------------------------------------
//  OnFrameRedirect
STDMETHODIMP CAnchoBrowserEvents::OnFrameRedirect(BSTR bstrOldUrl, BSTR bstrNewUrl)
{
  CComVariant vt[] = { bstrNewUrl, bstrOldUrl };
  DISPPARAMS disp = { vt, NULL, 2, 0 };
  return FireEvent(3, &disp, 2);
}

//----------------------------------------------------------------------------
//  FireDocumentEvent
HRESULT CAnchoBrowserEvents::FireEvent(DISPID dispid, DISPPARAMS* disp, unsigned int count)
{
  int nConnectionIndex;
  int nConnections = m_vec.GetSize();

  HRESULT hr = S_OK;
  Lock();
  for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
  {
    CComQIPtr<IDispatch> pDispatch = m_vec.GetAt(nConnectionIndex);
    if (pDispatch != NULL) {
      pDispatch->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, disp, NULL, NULL, NULL);
    }
  }
  Unlock();
  return hr;
}