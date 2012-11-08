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
  CAnchoProtocolSink() : m_FirstDataReceived(false), m_IsFrame(false) {}

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
  STDMETHODIMP BeginningTransaction(
    /* [in] */ LPCWSTR szURL,
    /* [in] */ LPCWSTR szHeaders,
    /* [in] */ DWORD dwReserved,
    /* [out] */ LPWSTR *pszAdditionalHeaders);

  STDMETHODIMP OnResponse(
    /* [in] */ DWORD dwResponseCode,
    /* [in] */ LPCWSTR szResponseHeaders,
    /* [in] */ LPCWSTR szRequestHeaders,
    /* [out] */ LPWSTR *pszAdditionalRequestHeaders);

  STDMETHODIMP ReportProgress(
    /* [in] */ ULONG ulStatusCode,
    /* [in] */ LPCWSTR szStatusText);

  STDMETHODIMP ReportData(
    /* [in] */ DWORD grfBSCF,
    /* [in] */ ULONG ulProgress,
    /* [in] */ ULONG ulProgressMax);

  STDMETHODIMP ReportResult(
    /* [in] */ HRESULT hrResult,
    /* [in] */ DWORD dwError,
    /* [in] */ LPCWSTR szResult);

  // -------------------------------------------------------------------------
  // Getters/setters
  boolean IsFrame() { return m_IsFrame; }

private:
  // -------------------------------------------------------------------------
  // Implementation
  LPVOID MakeSwitchParams(const BSTR param1, const BSTR param2 = L"");

  // -------------------------------------------------------------------------
  // Private members.
  std::wstring m_Url;
  boolean m_FirstDataReceived;
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
  // -------------------------------------------------------------------------
  // DocumentSink class (for sinking HTMLDocumentEvents2)
  class DocumentSink :
    public IDispEventImpl<1, DocumentSink, &DIID_HTMLDocumentEvents2, &LIBID_MSHTML, 4, 0>
  {
  public:
    // -------------------------------------------------------------------------
    // Constructor/destructor
    DocumentSink(IHTMLDocument2 *doc, DAnchoBrowserEvents* events, BSTR bstrUrl) :
      m_Doc(doc), m_Events(events), m_Url(bstrUrl)
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
    CComBSTR m_Url;
  };

  // -------------------------------------------------------------------------
  // IInternetProtocolRoot
  virtual STDMETHODIMP Continue(PROTOCOLDATA *pProtocolData);

public:
  // -------------------------------------------------------------------------
  // Destructor
  virtual ~CAnchoPassthruAPP();

protected:
  // -------------------------------------------------------------------------
  // Data members
  CComPtr<DAnchoBrowserEvents> mBrowserEvents;
  CComPtr<IWebBrowser2> m_Browser;
  std::set<CComPtr<IWebBrowser2> > m_FoundFrames;
};
