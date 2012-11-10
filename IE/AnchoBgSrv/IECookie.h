/****************************************************************************
 * IECookie.h : Declaration of the CIECookie
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once
//#include "Magpie.h"
#include "AnchoBgSrv_i.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


struct CookieParsedData
{
  CookieParsedData();
  void Reset();
  BOOL Add(char * p);

  UINT   curElement;
  char * name;
  char * value;
  char * hostPath;
  char * flags;
  char * expTimeLow;
  char * expTimeHigh;
  char * creationTimeLow;
  char * creationTimeHigh;
};

/*============================================================================
 * class CIECookie
 * Implements a cookie.
 */
class CIECookie;
typedef CComObject<CIECookie> CIECookieComObject;

class ATL_NO_VTABLE CIECookie :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIECookie, &CLSID_IECookie>,
	public IDispatchImpl<IIECookie, &IID_IIECookie, &LIBID_AnchoBgSrvLib,
                      /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
  DECLARE_NOT_AGGREGATABLE(CIECookie)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CIECookie)
	  COM_INTERFACE_ENTRY(IIECookie)
	  COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()

public:
  // -------------------------------------------------------------------------
  // static creator function
  static HRESULT CreateObject(CookieParsedData & cookieData, CIECookieComObject  *&  pRet);

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
	void FinalRelease();

public:
  // -------------------------------------------------------------------------
  // public members

public:
  // -------------------------------------------------------------------------
  // Getters.
public:
  // -------------------------------------------------------------------------
  // IIECookie methods. See .idl for description.
  STDMETHOD(get_name)(BSTR * pbsRet);
  STDMETHOD(get_value)(BSTR * pbsRet);
  STDMETHOD(get_domain)(BSTR * pbsRet);
  STDMETHOD(get_path)(BSTR * pbsRet);
  STDMETHOD(get_expirationDate)(VARIANT * aRet);
protected:
  // -------------------------------------------------------------------------
  // ctor
	CIECookie();

private:
  // -------------------------------------------------------------------------
  // Private methods.
	HRESULT Init(CookieParsedData & cookieData);

private:
  // -------------------------------------------------------------------------
  // Private members.
  CString     m_sName;
  CString     m_sValue;
  CString     m_sDomain;
  CString     m_sPath;
  FILETIME    m_ExpirationTime;
  FILETIME    m_CreationTime;


};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(IECookie), CIECookie)

