/****************************************************************************
 * AnchoBackground.cpp : Implementation of CAnchoAddonBackground
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoBackground.h"
#include "AnchoAddonService.h"
#include "AnchoBgSrvModule.h"
//#include "AnchoBackgroundConsole.h"
#include "ProtocolHandlerRegistrar.h"

/*============================================================================
 * class CAnchoAddonBackground
 */

//----------------------------------------------------------------------------
//  Init
HRESULT CAnchoAddonBackground::Init(
  LPCTSTR                     lpszThisPath,
  CAnchoAddonServiceCallback  * pAddonServiceCallback,
  BSTR                        bsID)
{
  // set service callback
  m_pAddonServiceCallback = pAddonServiceCallback;

  // set our ID
  m_bsID = bsID;

  // lookup ID in registry
  CRegKey regKey;
  CString sKey;
  sKey.Format(_T("%s\\%s"), s_AnchoExtensionsRegistryKey, bsID);
  LONG res = regKey.Open(HKEY_CURRENT_USER, sKey, KEY_READ);
  if (ERROR_SUCCESS != res)
  {
    return HRESULT_FROM_WIN32(res);
  }

  // get addon GUID
  CString sGUID;
  ULONG nChars = 37;  // length of a GUID + terminator
  res = regKey.QueryStringValue(s_AnchoExtensionsRegistryEntryGUID, sGUID.GetBuffer(nChars), &nChars);
  sGUID.ReleaseBuffer();
  if (ERROR_SUCCESS != res)
  {
    return HRESULT_FROM_WIN32(res);
  }

  // get addon path
  CString sPath;
  nChars = _MAX_PATH;
  LPTSTR pst = sPath.GetBuffer(nChars+1);
  res = regKey.QueryStringValue(s_AnchoExtensionsRegistryEntryPath, pst, &nChars);
  pst[nChars] = 0;
  PathAddBackslash(pst);
  sPath.ReleaseBuffer();
  if (ERROR_SUCCESS != res)
  {
    return HRESULT_FROM_WIN32(res);
  }
  if (!PathIsDirectory(sPath))
  {
    return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
  }

  // register a protocol handler for this extension
  // TODO: register a resource based handler depending on the usage of a resource DLL
  CStringW sRootURL;
  sRootURL.Format(L"%s://%s/", s_AnchoProtocolHandlerScheme, m_bsID);
  IF_FAILED_RET(CProtocolHandlerRegistrar::
    RegisterTemporaryFolderHandler(s_AnchoProtocolHandlerScheme, m_bsID, sPath));

  // init API
  IF_FAILED_RET(m_BackgroundAPI.Init(lpszThisPath, sRootURL, bsID, sGUID, sPath, pAddonServiceCallback));

  return S_OK;
}

//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonBackground::FinalConstruct()
{
  m_pAddonServiceCallback = NULL;
  return S_OK;
}

//----------------------------------------------------------------------------
//
void CAnchoAddonBackground::FinalRelease()
{
  m_BackgroundAPI.UnInit();
  if (m_pAddonServiceCallback)
  {
    // Notify addon service that we go away. Service needs this notification
    // to remove us from its internal map.
    m_pAddonServiceCallback->OnAddonFinalRelease(m_bsID);
  }
  CProtocolHandlerRegistrar::
    UnregisterTemporaryFolderHandler(s_AnchoProtocolHandlerScheme, m_bsID);
}

//----------------------------------------------------------------------------
//
void CAnchoAddonBackground::AddonServiceLost()
{
  m_pAddonServiceCallback = NULL;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::invokeExternalEventObject(BSTR aExtensionId, BSTR aEventName, LPDISPATCH aArgs, VARIANT* aRet)
{
  return m_BackgroundAPI.invokeEventObject(aEventName, -1, aArgs, aRet);
}
//----------------------------------------------------------------------------
//  IAnchoAddonBackground methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::AdviseInstance(ULONG * pulInstanceID)
{
  ENSURE_RETVAL(pulInstanceID);
  (*pulInstanceID) = m_NextInstanceID++;
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::UnadviseInstance(ULONG ulInstanceID)
{
  return m_BackgroundAPI.ReleaseContentAPI(ulInstanceID);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::GetContentAPI(ULONG ulInstanceID, LPDISPATCH* ppDisp)
{
  return m_BackgroundAPI.GetContentAPI(ulInstanceID, ppDisp);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::GetManifest(LPDISPATCH* ppDisp)
{
  ENSURE_RETVAL(ppDisp);
  return E_FAIL;
}

//----------------------------------------------------------------------------
//  IAnchoAddonBackground methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::log(BSTR bsSource, BSTR bsModuleID, VARIANT val)
{
  CComPtr<CLogWindowComObject> logWindow;
  IF_FAILED_RET(m_BackgroundAPI.GetLogWindow(logWindow.p));
  return logWindow->log(bsSource, bsModuleID, val);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::debug(BSTR bsSource, BSTR bsModuleID, VARIANT val)
{
  CComPtr<CLogWindowComObject> logWindow;
  IF_FAILED_RET(m_BackgroundAPI.GetLogWindow(logWindow.p));
  return logWindow->debug(bsSource, bsModuleID, val);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::info(BSTR bsSource, BSTR bsModuleID, VARIANT val)
{
  CComPtr<CLogWindowComObject> logWindow;
  IF_FAILED_RET(m_BackgroundAPI.GetLogWindow(logWindow.p));
  return logWindow->info(bsSource, bsModuleID, val);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::warn(BSTR bsSource, BSTR bsModuleID, VARIANT val)
{
  CComPtr<CLogWindowComObject> logWindow;
  IF_FAILED_RET(m_BackgroundAPI.GetLogWindow(logWindow.p));
  return logWindow->warn(bsSource, bsModuleID, val);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonBackground::error(BSTR bsSource, BSTR bsModuleID, VARIANT val)
{
  CComPtr<CLogWindowComObject> logWindow;
  IF_FAILED_RET(m_BackgroundAPI.GetLogWindow(logWindow.p));
  return logWindow->error(bsSource, bsModuleID, val);
}
