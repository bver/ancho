/****************************************************************************
 * IECookieManager.h : Declaration of the CIECookieManager
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once
//#include "resource.h"
//#include "Magpie.h"
#include "AnchoBgSrv_i.h"
#include "IECookie.h"
#include <vector>
#include <memory>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif
/*============================================================================*/
struct ACookieCallbackFunctor
{
  typedef std::auto_ptr<ACookieCallbackFunctor> APtr;
  virtual void operator()(CComVariant &aCookie)=0;
};
/*============================================================================
 * class CIECookieManager
 * Implements the cookie manager.
 */
class ATL_NO_VTABLE CIECookieManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIECookieManager, &CLSID_IECookieManager>,
	public IDispatchImpl<IIECookieManager, &IID_IIECookieManager, &LIBID_AnchoBgSrvLib,
                      /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
  DECLARE_NOT_AGGREGATABLE(CIECookieManager)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CIECookieManager)
	  COM_INTERFACE_ENTRY(IIECookieManager)
	  COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
	void FinalRelease();

public:
  // -------------------------------------------------------------------------
  // public members
  HRESULT OnCookieFileChanged(LPCTSTR lpszFileName);
  static HRESULT GetCookieFolder(CString& sPath);
  static void FileWatcher(void* args);

  void startWatching();
  void setNotificationCallback(ACookieCallbackFunctor::APtr aCallback)
  {
    m_OnCookiesChangedCallback = aCallback;
  }
public:
  // -------------------------------------------------------------------------
  // Getters.
public:
  // -------------------------------------------------------------------------
  // IIECookieManager methods. See .idl for description.
  STDMETHOD(enumCookies)(LPDISPATCH pCallback);
  STDMETHOD(removeCookie)(BSTR aUrl, BSTR aName);
  STDMETHOD(setCookie)(BSTR aUrl, BSTR aName, BSTR aData);
  STDMETHOD(getCookie)(BSTR aUrl, BSTR aName, VARIANT *aData);

protected:
  // -------------------------------------------------------------------------
  // ctor
	CIECookieManager();

private:
  // -------------------------------------------------------------------------
  // types and classes.
  typedef std::vector<CComPtr<CIECookieComObject> >  CCookieArray;

  class ParseBuffer
  {
  public:
    ParseBuffer(size_t size);
    ParseBuffer(size_t size, char * buffer);
    ~ParseBuffer();
    char & operator [](size_t pos);
    operator char * ();
    operator void * ();
  private:
    size_t  m_Size;
    char  * m_Buffer;
  };
  // Helper window for messaging between file watcher thread and UI thread
  #define WM_ONCOOKIEMESSAGE  (WM_APP+123)
  class CCookieHelperWindow :  public CWindowImpl<CCookieHelperWindow, CWindow, CWinTraits<WS_POPUP>>
  {
  public:
    CIECookieManager * m_IECookieManager;
    ~CCookieHelperWindow()
    {
      ATLTRACE(L"ANCHO IECOOKIEMANAGER::HELPERWINDOW DESTRUCTOR\n");
    }
  public:
    DECLARE_WND_CLASS(_T("CIECookiManager::CCookieHelperWindow"))

    BEGIN_MSG_MAP(CCookieHelperWindow)
      MESSAGE_HANDLER(WM_ONCOOKIEMESSAGE, OnCookieMessage)
    END_MSG_MAP()
    LRESULT OnCookieMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  };

private:
  // -------------------------------------------------------------------------
  // Private methods.
  HRESULT ParseCookieFile(LPCTSTR lpszFilename, CCookieArray & cookies);
  HRESULT ParseCookies(ParseBuffer & parseBuffer, size_t sLen, CCookieArray & cookies);
  HRESULT ReadCookieFile(LPCWSTR url, CCookieArray & cookies);

private:
  // -------------------------------------------------------------------------
  // Private members.
  //CComPtr<IDispatch> m_pOnCookiesChangedCallback;
  CCookieHelperWindow m_CookieHelperWindow;

  ACookieCallbackFunctor::APtr m_OnCookiesChangedCallback;
};

OBJECT_ENTRY_AUTO(__uuidof(IECookieManager), CIECookieManager)

