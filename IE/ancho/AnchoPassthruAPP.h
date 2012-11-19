/****************************************************************************
 * AnchoPassthruAPP.h : Declaration of the CAnchoPassthruAPP
 * Copyright 2012 Salsita (http://www.salsitasoft.com).
 * Author: Matthew Gertner <matthew@salsitasoft.com>
 ****************************************************************************/

#pragma once

#include "ProtocolImpl.h"
#include "AnchoBrowserEvents.h"

#include <string>
typedef std::basic_string<TCHAR> tstring;
#include <set>


/*============================================================================
 * class CAnchoProtocolSink
 */
class CAnchoProtocolSink :
  public PassthroughAPP::CInternetProtocolSinkWithSP<CAnchoProtocolSink>,
  public IHttpNegotiate
{
  friend class CAnchoStartPolicy;
  typedef PassthroughAPP::CInternetProtocolSinkWithSP<CAnchoProtocolSink> BaseClass;

public:

  // -------------------------------------------------------------------------
  // Constructor
  CAnchoProtocolSink() : m_IsFrame(false) {}

  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CAnchoProtocolSink)
    COM_INTERFACE_ENTRY(IHttpNegotiate)
    COM_INTERFACE_ENTRY_CHAIN(BaseClass)
  END_COM_MAP()

  BEGIN_SERVICE_MAP(CAnchoProtocolSink)
    SERVICE_ENTRY(IID_IHttpNegotiate)
  END_SERVICE_MAP()

  // -------------------------------------------------------------------------
  // IHttpNegotiate
  STDMETHOD(BeginningTransaction)(
    /* [in] */ LPCWSTR szURL,
    /* [in] */ LPCWSTR szHeaders,
    /* [in] */ DWORD dwReserved,
    /* [out] */ LPWSTR *pszAdditionalHeaders);

  STDMETHOD(OnResponse)(
    /* [in] */ DWORD dwResponseCode,
    /* [in] */ LPCWSTR szResponseHeaders,
    /* [in] */ LPCWSTR szRequestHeaders,
    /* [out] */ LPWSTR *pszAdditionalRequestHeaders);

  STDMETHOD(ReportProgress)(
    /* [in] */ ULONG ulStatusCode,
    /* [in] */ LPCWSTR szStatusText);

  STDMETHOD(ReportData)(
    /* [in] */ DWORD grfBSCF,
    /* [in] */ ULONG ulProgress,
    /* [in] */ ULONG ulProgressMax);

  STDMETHOD(ReportResult)(
    /* [in] */ HRESULT hrResult,
    /* [in] */ DWORD dwError,
    /* [in] */ LPCWSTR szResult);

  // -------------------------------------------------------------------------
  // Public interface
  boolean IsFrame() { return m_IsFrame; }
  // Free the memory associated with the params allocated when calling Switch().
  void FreeSwitchParams(BSTR* params);

private:
  // -------------------------------------------------------------------------
  // Implementation
  // Allocate the correct parameters for calling Switch().
  LPVOID InitSwitchParams(const BSTR param1, const BSTR param2 = L"");

  // -------------------------------------------------------------------------
  // Private members.
  std::wstring m_Url;
  boolean m_IsFrame;
};

class CAnchoPassthruAPP;

/*============================================================================
 * class CAnchoStartPolicy
 */
class CAnchoStartPolicy :
  public PassthroughAPP::CustomSinkStartPolicy<CAnchoPassthruAPP, CAnchoProtocolSink>
{
public:
  HRESULT OnStart(LPCWSTR szUrl,
    IInternetProtocolSink *pOIProtSink, IInternetBindInfo *pOIBindInfo,
    DWORD grfPI, HANDLE_PTR dwReserved,
    IInternetProtocol* pTargetProtocol);

  HRESULT OnStartEx(IUri* pUri,
    IInternetProtocolSink *pOIProtSink, IInternetBindInfo *pOIBindInfo,
    DWORD grfPI, HANDLE_PTR dwReserved,
    IInternetProtocolEx* pTargetProtocol);
};

/*============================================================================
 * class CAnchoPassthruAPP
 */
class CAnchoPassthruAPP :
  public PassthroughAPP::CInternetProtocol<CAnchoStartPolicy>
{
private:
  typedef std::vector<std::pair<std::wstring, std::wstring> > RedirectList;

  // -------------------------------------------------------------------------
  // DocumentSink class (for sinking HTMLDocumentEvents2)
  class DocumentSink :
    public IDispEventImpl<1, DocumentSink, &DIID_HTMLDocumentEvents2, &LIBID_MSHTML, 4, 0>
  {
  public:
    // -------------------------------------------------------------------------
    // Constructor/destructor
    DocumentSink(IInternetProtocolRoot* app, IHTMLDocument2 *doc, DAnchoBrowserEvents* events, BSTR bstrUrl) :
      m_APP(app), m_Doc(doc), m_Events(events), m_Url(bstrUrl)
    {
    }
    ~DocumentSink();

    // -------------------------------------------------------------------------
    // Event map
    BEGIN_SINK_MAP(DocumentSink)
      SINK_ENTRY_EX(1, DIID_HTMLDocumentEvents2, DISPID_READYSTATECHANGE, OnReadyStateChange)
    END_SINK_MAP()

    // -------------------------------------------------------------------------
    // Methods
    STDMETHOD_(void, OnReadyStateChange)(IHTMLEventObj* ev);

  protected:
    // -------------------------------------------------------------------------
    // Data members
    CComPtr<IHTMLDocument2> m_Doc;
    CComPtr<DAnchoBrowserEvents> m_Events;
    // Hold a pointer to the APP so we don't get freed too early.
    CComPtr<IInternetProtocolRoot> m_APP;
    CComBSTR m_Url;
  };

  // -------------------------------------------------------------------------
  // IInternetProtocolRoot
  STDMETHOD(Continue)(PROTOCOLDATA *pProtocolData);

public:
  // -------------------------------------------------------------------------
  // Destructor
  CAnchoPassthruAPP() : m_DocSink(NULL), m_IsMainFrame(false) {}
  virtual ~CAnchoPassthruAPP();

protected:
  // -------------------------------------------------------------------------
  // Data members
  CComQIPtr<DAnchoBrowserEvents> m_BrowserEvents;
  CComPtr<IHTMLDocument2> m_Doc;
  bool m_IsMainFrame;
  CComPtr<IWebBrowser2> m_Browser;
  std::set<CComPtr<IWebBrowser2> > m_FoundFrames;
  DocumentSink* m_DocSink;
  RedirectList m_Redirects;
};
