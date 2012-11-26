/****************************************************************************
 * IECookie.cpp : Implementation of CIECookie
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "StdAfx.h"
#include "IECookie.h"

#include <ctime>

#define EPOCH_DIFF 0x019DB1DED53E8000LL /* 116444736000000000 nsecs */
#define RATE_DIFF 10000000 /* 100 nsecs */

typedef INT64 filetime_t;

/* Convert a UNIX time_t into a Windows filetime_t */
filetime_t unixTimeToFileTime(time_t utime) {
        INT64 tconv = ((INT64)utime * RATE_DIFF) + EPOCH_DIFF;
        return tconv;
}

/* Convert a Windows filetime_t into a UNIX time_t */
time_t fileTimeToUnixTime(filetime_t ftime) {
        INT64 tconv = (ftime - EPOCH_DIFF) / RATE_DIFF;
        return (time_t)tconv;
}


CookieParsedData::CookieParsedData()
{
  Reset();
}

void CookieParsedData::Reset()
{
  memset(this, 0, sizeof(CookieParsedData));
}

BOOL CookieParsedData::Add(char * p)
{
  if (curElement > 7)
  {
    return FALSE;
  }
  char ** pSet = &name + curElement;
  *pSet = p;
  ++curElement;
  return TRUE;
}


/*============================================================================
 * class CIECookie
 */

//----------------------------------------------------------------------------
//  static CreateObject
HRESULT CIECookie::CreateObject(CookieParsedData & cookieData, CIECookieComObject  *&  pRet)
{
  CIECookieComObject *newObject = pRet = NULL;
  IF_FAILED_RET(CIECookieComObject::CreateInstance(&newObject));
  newObject->AddRef();

  HRESULT hr = E_FAIL;
  hr = newObject->Init(cookieData);
  if(FAILED(hr))
  {
    newObject->Release();
    return hr;
  }

  pRet = newObject;
  return S_OK;
}

//----------------------------------------------------------------------------
//  CTOR
CIECookie::CIECookie()
{
}

//----------------------------------------------------------------------------
//  FinalConstruct
HRESULT CIECookie::FinalConstruct()
{
	return S_OK;
}

//----------------------------------------------------------------------------
//  FinalRelease
void CIECookie::FinalRelease()
{
}

//----------------------------------------------------------------------------
//  Init
HRESULT CIECookie::Init(CookieParsedData & cookieData)
{
  if ( !cookieData.creationTimeHigh || !cookieData.creationTimeLow
    || !cookieData.expTimeHigh || !cookieData.expTimeLow
    || !cookieData.hostPath || !cookieData.name
    || !cookieData.value)
  {
    return E_INVALIDARG;
  }
  m_sName = CA2W(cookieData.name);
  m_sValue = CA2W(cookieData.value);

  // hostPath has format [domain]/[path] where the slash is part of the path.
  char* slash = strchr(cookieData.hostPath, '/');
  if (slash) {
    m_sPath = CA2W(slash);
    *slash = 0;
  }
  m_sDomain = CA2W(cookieData.hostPath);

  m_ExpirationTime.dwLowDateTime = atol(cookieData.expTimeLow);
  m_ExpirationTime.dwHighDateTime = atol(cookieData.expTimeHigh);
  m_CreationTime.dwLowDateTime = atol(cookieData.creationTimeLow);
  m_CreationTime.dwHighDateTime = atol(cookieData.creationTimeHigh);
  return S_OK;
}

STDMETHODIMP CIECookie::get_name(BSTR * pbsRet)
{
  ENSURE_RETVAL(pbsRet);
  (*pbsRet) = m_sName.AllocSysString();
  return S_OK;
}

STDMETHODIMP CIECookie::get_value(BSTR * pbsRet)
{
  ENSURE_RETVAL(pbsRet);
  (*pbsRet) = m_sValue.AllocSysString();
  return S_OK;
}

STDMETHODIMP CIECookie::get_domain(BSTR * pbsRet)
{
  ENSURE_RETVAL(pbsRet);
  (*pbsRet) = m_sDomain.AllocSysString();
  return S_OK;
}

STDMETHODIMP CIECookie::get_path(BSTR * pbsRet)
{
  ENSURE_RETVAL(pbsRet);
  (*pbsRet) = m_sPath.AllocSysString();
  return S_OK;
}

STDMETHODIMP CIECookie::get_expirationDate(VARIANT * aRet)
{
  ENSURE_RETVAL(aRet);
  aRet->vt = VT_R8;
  aRet->dblVal = static_cast<double>(fileTimeToUnixTime(*reinterpret_cast<filetime_t*>(&m_ExpirationTime)));
  return S_OK;
}
