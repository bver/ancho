/****************************************************************************
 * AnchoBackground.cpp : Implementation of CAnchoBackgroundAPI
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoBackground.h"
#include "AnchoAddonService.h"
//#include "AnchoBackgroundConsole.h"

/*============================================================================
 * class CAnchoBackgroundAPI
 */

#ifndef MAGPIE_REGISTERED
// need the function info for logging in case magpie is not registered
_ATL_FUNC_INFO CAnchoBackgroundAPI::info_Console_LogFunction = {CC_STDCALL,VT_EMPTY,2,{VT_VARIANT,VT_BSTR}};
#endif

// string identifyer used in console logging for background
LPCTSTR CAnchoBackgroundAPI::s_BackgroundLogIdentifyer = _T("background");

//----------------------------------------------------------------------------
//  Init
HRESULT CAnchoBackgroundAPI::Init(LPCTSTR lpszThisPath, LPCTSTR lpszRootURL, BSTR bsID, LPCTSTR lpszGUID, LPCTSTR lpszPath)
{
  // set our ID
  m_bsID = bsID;
  m_GUID = lpszGUID;

  m_sRootURL = lpszRootURL;

  CString sPath(lpszPath);

  // create logger window
  IF_FAILED_RET(CLogWindow::CreateLogWindow(&m_LogWindow.p));

  // create a magpie instance
#ifdef MAGPIE_REGISTERED
  IF_FAILED_RET(m_Magpie.CoCreateInstance(CLSID_MagpieApplication));
#else
  // Load magpie from the same path where this exe file is.
  CString s(lpszThisPath);
  s += _T("Magpie.dll");
  HMODULE hModMagpie = ::LoadLibrary(s);
  if (!hModMagpie)
  {
    return E_FAIL;
  }
  fnCreateMagpieInstance CreateMagpieInstance = (fnCreateMagpieInstance)::GetProcAddress(hModMagpie, "CreateMagpieInstance");
  if (!CreateMagpieInstance)
  {
    return E_FAIL;
  }
  IF_FAILED_RET(CreateMagpieInstance(&m_Magpie));
#endif
  // add a loader for scripts in the extension filesystem
  IF_FAILED_RET(m_Magpie->AddFilesystemScriptLoader((LPWSTR)(LPCWSTR)sPath));

  // add a loder for scripts in this exe file
  IF_FAILED_RET(m_Magpie->AddResourceScriptLoader((ULONG)_AtlModule.GetResourceInstance()));

  // advise logger
  IF_FAILED_RET(AtlAdvise(m_Magpie, (IUnknown*)(CAnchoAddonBackgroundLogger*)(this), DIID__IMagpieLoggerEvents, &m_dwMagpieSinkCookie));

  // load manifest
  CString sManifestFilename;
  sManifestFilename.Format(_T("%smanifest.json"), sPath);
  CAtlFile f;
  IF_FAILED_RET(f.Create(sManifestFilename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING));
  ULONGLONG nLen = 0;
  IF_FAILED_RET(f.GetSize(nLen));
  // limit to 4gb
  if (nLen > 0x00000000ffffffff)
  {
    return E_OUTOFMEMORY;
  }
  DWORD dwLen = (DWORD)nLen;
  CStringA sManifest("exports.manifest = ");
  DWORD strLen = (DWORD)sManifest.GetLength();
  IF_FAILED_RET(f.Read(sManifest.GetBuffer(dwLen + strLen) + strLen, dwLen));
  sManifest.ReleaseBuffer(dwLen + strLen);
  sManifest += _T(';');
  IF_FAILED_RET(m_Magpie->RunScript(L"manifest", (LPWSTR)(LPCWSTR)CA2W(sManifest)));

  // set ourselfs in magpie as a global accessible object
  IF_FAILED_RET(m_Magpie->AddExtension((LPWSTR)s_AnchoGlobalAPIObjectName, (IAnchoBackgroundAPI*)this));

  // initialize Ancho API
  // api.js will do now additional initialization, like looking at the manifest
  // and creating a background page.
  IF_FAILED_RET(m_Magpie->Run((LPWSTR)s_AnchoMainAPIModuleID));

  return S_OK;
}

//----------------------------------------------------------------------------
//  
HRESULT CAnchoBackgroundAPI::FinalConstruct()
{
  return S_OK;
}

//----------------------------------------------------------------------------
//  
void CAnchoBackgroundAPI::FinalRelease()
{
  UnInit();
}

//----------------------------------------------------------------------------
//  
void CAnchoBackgroundAPI::UnInit()
{
  if (m_BackgroundWindow)
  {
    m_BackgroundWindow->DestroyWindow();
    m_BackgroundWindow.Release();
  }
  if (m_LogWindow)
  {
    m_LogWindow->DestroyWindow();
    m_LogWindow.Release();
  }
  if (m_Magpie)
  {
    if (m_dwMagpieSinkCookie)
    {
      AtlUnadvise(m_Magpie, DIID__IMagpieLoggerEvents, m_dwMagpieSinkCookie);
      m_dwMagpieSinkCookie = 0;
    }
    m_Magpie->Shutdown();
    m_Magpie.Release();
  }
}

//----------------------------------------------------------------------------
//  
HRESULT CAnchoBackgroundAPI::GetLogWindow(CLogWindowComObject * & pLogWindow)
{
  if (!m_LogWindow)
  {
    return E_UNEXPECTED;
  }
  return m_LogWindow.CopyTo(&pLogWindow);
}

//----------------------------------------------------------------------------
//  
HRESULT CAnchoBackgroundAPI::GetContentAPI(ULONG ulInstanceID, LPDISPATCH* ppDisp)
{
  ENSURE_RETVAL(ppDisp);
  if (!m_Magpie)
  {
    return E_UNEXPECTED;
  }

  CIDispatchHelper script;
  IF_FAILED_RET(GetMainModuleExportsScript(script));

  CComVariant vtRet, vtInstanceID(ulInstanceID);
  DISPPARAMS params = {&vtInstanceID, NULL, 1, 0};
  IF_FAILED_RET(script.Call((LPOLESTR)s_AnchoFnGetContentAPI, &params, &vtRet));
  if (vtRet.vt != VT_DISPATCH)
  {
    return E_FAIL;
  }
  return vtRet.pdispVal->QueryInterface(IID_IDispatch, (void**)ppDisp);
}

//----------------------------------------------------------------------------
//  
HRESULT CAnchoBackgroundAPI::ReleaseContentAPI(ULONG ulInstanceID)
{
  if (!m_Magpie)
  {
    return E_UNEXPECTED;
  }

  // get the main api module
  CIDispatchHelper script;
  IF_FAILED_RET(GetMainModuleExportsScript(script));

  CComVariant vtInstanceID(ulInstanceID);
  DISPPARAMS params = {&vtInstanceID, NULL, 1, 0};
  IF_FAILED_RET(script.Call((LPOLESTR)s_AnchoFnReleaseContentAPI, &params));
  return S_OK;
}

//----------------------------------------------------------------------------
//  
BOOL CAnchoBackgroundAPI::GetURL(CStringW & sURL)
{
  CString s;
  DWORD dwLength = INTERNET_MAX_URL_LENGTH;
  BOOL b = InternetCombineUrlW(m_sRootURL, sURL, s.GetBuffer(INTERNET_MAX_URL_LENGTH), &dwLength, 0);
  s.ReleaseBuffer();
  if (b)
  {
    sURL = s;
  }
  return b;
}

//----------------------------------------------------------------------------
//  
HRESULT CAnchoBackgroundAPI::GetMainModuleExportsScript(CIDispatchHelper & script)
{
  // get the main api module
  CComPtr<IMagpieModuleRestricted> mainModule;
  IF_FAILED_RET(m_Magpie->GetModuleObject((LPOLESTR)s_AnchoMainAPIModuleID, &mainModule));

  CComPtr<IDispatch> mainModuleExports;
  IF_FAILED_RET(mainModule->GetExportsObject(&mainModuleExports));

  script = mainModuleExports;
  return S_OK;
}

//----------------------------------------------------------------------------
//  IAnchoBackgroundAPI methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  
STDMETHODIMP CAnchoBackgroundAPI::get_id(BSTR * pVal)
{
  ENSURE_RETVAL(pVal);
  return m_bsID.CopyTo(pVal);
}

//----------------------------------------------------------------------------
//  
STDMETHODIMP CAnchoBackgroundAPI::get_guid(BSTR * pVal)
{
  ENSURE_RETVAL(pVal);
  (*pVal) = m_GUID.AllocSysString();
  return S_OK;
}

//----------------------------------------------------------------------------
//  
STDMETHODIMP CAnchoBackgroundAPI::startBackgroundWindow(BSTR bsPartialURL)
{
  // it's safe to call this method multiple times, anyhow the window
  // will be created only once
  if (m_BackgroundWindow)
  {
    return S_OK;
  }
  CStringW sURL(bsPartialURL);
  if (!GetURL(sURL))
  {
    return E_FAIL;
  }

  // get the main api module and inject it into the background page
  CComPtr<IMagpieModuleRestricted> mainModule;
  IF_FAILED_RET(m_Magpie->GetModuleObject((LPOLESTR)s_AnchoMainAPIModuleID, &mainModule));

  CComPtr<IDispatch> mainModuleExports;
  IF_FAILED_RET(mainModule->GetExportsObject(&mainModuleExports));

  CComVariant vt;
  IF_FAILED_RET(mainModuleExports.GetPropertyByName(s_AnchoBackgroundPageAPIName, &vt));
  if (vt.vt != VT_DISPATCH)
  {
    return E_FAIL;
  }

  IF_FAILED_RET(CBackgroundWindow::CreateBackgroundWindow(vt.pdispVal, sURL, &m_BackgroundWindow.p));
  return S_OK;
}

//----------------------------------------------------------------------------
//  _IMagpieLoggerEvents methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnLog(VARIANT val, BSTR bsModuleID)
{
  if (m_LogWindow)
  {
    m_LogWindow->log(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, val);
  }
}

//----------------------------------------------------------------------------
//  
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnDebug(VARIANT val, BSTR bsModuleID)
{
  if (m_LogWindow)
  {
    m_LogWindow->debug(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, val);
  }
}

//----------------------------------------------------------------------------
//  
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnInfo(VARIANT val, BSTR bsModuleID)
{
  if (m_LogWindow)
  {
    m_LogWindow->info(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, val);
  }
}

//----------------------------------------------------------------------------
//  
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnWarn(VARIANT val, BSTR bsModuleID)
{
  if (m_LogWindow)
  {
    m_LogWindow->warn(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, val);
  }
}

//----------------------------------------------------------------------------
//  
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnError(VARIANT val, BSTR bsModuleID)
{
  if (m_LogWindow)
  {
    m_LogWindow->error(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, val);
  }
}
