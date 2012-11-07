/****************************************************************************
 * AnchoPassthruAPP.cpp : Implementation of CAnchoPassthruAPP
 * Copyright 2012 Salsita (http://www.salsitasoft.com).
 * Author: Matthew Gertner <matthew@salsitasoft.com>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoPassthruAPP.h"

#include "libbhohelper.h"

#include <atlbase.h>
#include <WinInet.h>
#include <htiframe.h>

#define ANCHO_SWITCH_BASE 50000
#define ANCHO_SWITCH_REPORT_DATA ANCHO_SWITCH_BASE+1
#define ANCHO_SWITCH_REPORT_RESULT ANCHO_SWITCH_BASE+2
#define ANCHO_SWITCH_REDIRECT ANCHO_SWITCH_BASE+3

/*============================================================================
 * class CAnchoStartPolicy
 */

//----------------------------------------------------------------------------
//  OnStart
HRESULT CAnchoStartPolicy::OnStart(
  LPCWSTR szUrl, IInternetProtocolSink *pOIProtSink,
  IInternetBindInfo *pOIBindInfo,  DWORD grfPI, HANDLE_PTR dwReserved,
  IInternetProtocol* pTargetProtocol)
{
  GetSink()->m_Url = szUrl;
  return __super::OnStart(szUrl, pOIProtSink, pOIBindInfo, grfPI, dwReserved, pTargetProtocol);
}

//----------------------------------------------------------------------------
//  OnStartEx
HRESULT CAnchoStartPolicy::OnStartEx(
  IUri* pUri, IInternetProtocolSink *pOIProtSink,
  IInternetBindInfo *pOIBindInfo,  DWORD grfPI, HANDLE_PTR dwReserved,
  IInternetProtocolEx* pTargetProtocol)
{
  ATLASSERT(pUri != NULL);
  BSTR rawUri;
  HRESULT hr = pUri->GetRawUri(&rawUri);
  GetSink()->m_Url = std::wstring(rawUri, ::SysStringLen(rawUri));
  return __super::OnStartEx(pUri, pOIProtSink, pOIBindInfo, grfPI, dwReserved, pTargetProtocol);
}

/*============================================================================
 * class CAnchoProtocolSink
 */

//----------------------------------------------------------------------------
//  BeginningTransaction
STDMETHODIMP CAnchoProtocolSink::BeginningTransaction(
  /* [in] */ LPCWSTR szURL,
  /* [in] */ LPCWSTR szHeaders,
  /* [in] */ DWORD dwReserved,
  /* [out] */ LPWSTR *pszAdditionalHeaders)
{
  // If we have a bind context then we know that this request is for a document loading into a frame.
  LPOLESTR bind_ctx_string = NULL;
  CComPtr<IBindCtx> bind_ctx;
  ULONG count;
  GetBindString(BINDSTRING_PTR_BIND_CONTEXT, &bind_ctx_string, 1, &count);
  m_IsFrame = (bind_ctx_string != NULL);

  if (pszAdditionalHeaders)
  {
    *pszAdditionalHeaders = 0;
  }

  CComPtr<IHttpNegotiate> spHttpNegotiate;
  IF_FAILED_RET(QueryServiceFromClient(&spHttpNegotiate));

  HRESULT hr = spHttpNegotiate ?
    spHttpNegotiate->BeginningTransaction(szURL, szHeaders,
      dwReserved, pszAdditionalHeaders) :
    S_OK;
  IF_FAILED_RET(hr);

  return S_OK;
}

//----------------------------------------------------------------------------
//  OnResponse
STDMETHODIMP CAnchoProtocolSink::OnResponse(
  /* [in] */ DWORD dwResponseCode,
  /* [in] */ LPCWSTR szResponseHeaders,
  /* [in] */ LPCWSTR szRequestHeaders,
  /* [out] */ LPWSTR *pszAdditionalRequestHeaders)
{
  HRESULT hr;
  CComPtr<IHttpNegotiate> spHttpNegotiate;
  QueryServiceFromClient(&spHttpNegotiate);

  if (pszAdditionalRequestHeaders)
  {
    *pszAdditionalRequestHeaders = 0;
  }

  hr = spHttpNegotiate ?
    spHttpNegotiate->OnResponse(dwResponseCode, szResponseHeaders,
      szRequestHeaders, pszAdditionalRequestHeaders) :
    S_OK;

  return hr;
}

//----------------------------------------------------------------------------
//  ReportProgress
STDMETHODIMP CAnchoProtocolSink::ReportProgress(
  /* [in] */ ULONG ulStatusCode,
  /* [in] */ LPCWSTR szStatusText)
{
  if (ulStatusCode == BINDSTATUS_REDIRECTING) {
    PROTOCOLDATA data;
    data.grfFlags = PD_FORCE_SWITCH;
    data.dwState = ANCHO_SWITCH_REDIRECT;
    data.pData = MakeSwitchParams((BSTR) m_Url.c_str(), (BSTR) szStatusText);
    data.cbData = sizeof(BSTR*);
    AddRef();
    Switch(&data);

    m_Url = szStatusText;
  }
  
  ATLASSERT(m_spInternetProtocolSink != 0);
  HRESULT hr = m_spInternetProtocolSink ?
    m_spInternetProtocolSink->ReportProgress(ulStatusCode, szStatusText) :
    S_OK;

  return hr;
}

//----------------------------------------------------------------------------
//  ReportData
STDMETHODIMP CAnchoProtocolSink::ReportData(
	/* [in] */ DWORD grfBSCF,
	/* [in] */ ULONG ulProgress,
	/* [in] */ ULONG ulProgressMax)
{
  if (!m_FirstDataReceived) {
    // Sometimes BSCF_FIRSTDATANOTIFICATION is received more than once.
    m_FirstDataReceived = true;

    PROTOCOLDATA data;
    data.grfFlags = PD_FORCE_SWITCH;
    data.dwState = ANCHO_SWITCH_REPORT_DATA;
    data.pData = MakeSwitchParams((BSTR) m_Url.c_str());
    data.cbData = sizeof(BSTR*);
    AddRef();
    Switch(&data);
  }

  return m_spInternetProtocolSink->ReportData(grfBSCF, ulProgress, ulProgressMax);
}

//----------------------------------------------------------------------------
//  ReportResult
STDMETHODIMP CAnchoProtocolSink::ReportResult(
	/* [in] */ HRESULT hrResult,
	/* [in] */ DWORD dwError,
	/* [in] */ LPCWSTR szResult)
{
  if (hrResult == 0) {
    PROTOCOLDATA data;
    data.grfFlags = PD_FORCE_SWITCH;
    data.dwState = ANCHO_SWITCH_REPORT_RESULT;
    data.pData = MakeSwitchParams((BSTR) m_Url.c_str());
    data.cbData = sizeof(BSTR*);
    AddRef();
    Switch(&data);
  }

  return m_spInternetProtocolSink->ReportResult(hrResult, dwError, szResult);
}

//----------------------------------------------------------------------------
//  MakeSwitchParams
LPVOID CAnchoProtocolSink::MakeSwitchParams(const BSTR param1, const BSTR param2)
{
  BSTR* params = new BSTR[2];
  params[0] = ::SysAllocString(param1);
  params[1] = ::SysAllocString(param2);
  return (LPVOID) params;
}

/*============================================================================
 * class CAnchoPassthruAPP::DocumentSink
 */

//----------------------------------------------------------------------------
//  Destructor
CAnchoPassthruAPP::DocumentSink::~DocumentSink()
{
  if (m_Doc) {
    DispEventUnadvise(m_Doc);
  }
}

//----------------------------------------------------------------------------
//  CAnchoPassthruAPP::OnReadyStateChange
STDMETHODIMP_(void) CAnchoPassthruAPP::DocumentSink::OnReadyStateChange(IHTMLEventObj* ev)
{
  BSTR readyState;
  m_Doc->get_readyState(&readyState);

  if (wcscmp(readyState, L"complete") == 0) {
    CComBSTR loc;
    // Now that the document is loaded, check again to see if we are the main frame.
    HRESULT hr = m_Doc->get_URL(&loc);
    BOOL isMainFrame = SUCCEEDED(hr) && (loc == m_Url);
    DispEventUnadvise(m_Doc);
    m_Doc = NULL;
    m_Events->OnFrameEnd(m_Url, isMainFrame);
  }
  ::SysFreeString(readyState);
}

/*============================================================================
 * class CAnchoPassthruAPP
 */

//----------------------------------------------------------------------------
//  Destructor
CAnchoPassthruAPP::~CAnchoPassthruAPP()
{
}

//----------------------------------------------------------------------------
//  Continue
STDMETHODIMP CAnchoPassthruAPP::Continue(PROTOCOLDATA* data)
{
  if (data->dwState >= ANCHO_SWITCH_BASE) {
    BSTR* params = (BSTR*) data->pData;
    ATLASSERT(params);
    CComBSTR bstrUrl = params[0];
    CComBSTR bstrAdditional = params[1];
    ::SysFreeString(params[0]);
    ::SysFreeString(params[1]);
    delete [] params;

    CComPtr<IWindowForBindingUI> windowForBindingUI;
    CComPtr<CAnchoProtocolSink> pSink = GetSink();
    // Release the reference we added when calling Switch().
    pSink->InternalRelease();

    pSink->QueryServiceFromClient(IID_IWindowForBindingUI, &windowForBindingUI);
    if (windowForBindingUI) {
      HWND hwnd;
      HRESULT hr = windowForBindingUI->GetWindow(IID_IAuthenticate, &hwnd);
      IF_FAILED_RET(hr);

      CComPtr<IHTMLDocument2> doc;
      hr = getHTMLDocumentForHWND(hwnd, &doc);
      IF_FAILED_RET(hr);

      CComPtr<IWebBrowser2> browser;
      hr = getBrowserForHTMLDocument(doc, &browser);
      IF_FAILED_RET(hr);

      CComBSTR url;
      hr = browser->get_LocationURL(&url);
      IF_FAILED_RET(hr);
      bool isMainFrame = (url == bstrUrl);

      // We only want to handle the top-level request and any frames, not subordinate
      // requests like images. Usually the desired requests will have a bind context,
      // but in the case of a page refresh, the top-level request annoyingly doesn't,
      // so we check if the URL of the request matches the URL of the browser to handle
      // that case.
      if (!(pSink->IsFrame()) && !isMainFrame) {
        return S_OK;
      }

      CComVariant var;
      hr = browser->GetProperty(L"_anchoBrowserEvents", &var);
      IF_FAILED_RET(hr);

      CComQIPtr<DAnchoBrowserEvents> events(var.pdispVal);
      if (!events) {
        return E_FAIL;
      }

      if (data->dwState == ANCHO_SWITCH_REPORT_DATA) {
        hr = events->OnFrameStart(bstrUrl, isMainFrame);
        IF_FAILED_RET(hr);

        CComBSTR readyState;
        doc->get_readyState(&readyState);
        if (wcscmp(readyState, L"complete") == 0) {
          hr = events->OnFrameEnd(bstrUrl, isMainFrame);
          IF_FAILED_RET(hr);
        }
        else {
          DocumentSink * docSink = new DocumentSink(doc, events, bstrUrl);
          docSink->DispEventAdvise(doc);
        }
      }
      else if (data->dwState == ANCHO_SWITCH_REDIRECT) {
        hr = events->OnFrameRedirect(bstrUrl, bstrAdditional);
        IF_FAILED_RET(hr);
      }
    }
  }
  return __super::Continue(data);
}
