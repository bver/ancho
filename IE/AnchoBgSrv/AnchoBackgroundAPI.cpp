/****************************************************************************
 * AnchoBackground.cpp : Implementation of CAnchoBackgroundAPI
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoBackground.h"
#include "AnchoAddonService.h"
//#include "AnchoBackgroundConsole.h"

#include <algorithm>


/*============================================================================
 * class CAnchoBackgroundAPI
 */

#ifndef MAGPIE_REGISTERED
// need the function info for logging in case magpie is not registered
_ATL_FUNC_INFO CAnchoBackgroundAPI::info_Console_LogFunction = {CC_STDCALL,VT_EMPTY,2,{VT_BSTR,VT_ARRAY|VT_VARIANT}};
#endif

// string identifyer used in console logging for background
LPCTSTR CAnchoBackgroundAPI::s_BackgroundLogIdentifyer = _T("background");

//----------------------------------------------------------------------------
//  Init
HRESULT CAnchoBackgroundAPI::Init(LPCTSTR lpszThisPath, LPCTSTR lpszRootURL, BSTR bsID, LPCTSTR lpszGUID, LPCTSTR lpszPath, IAnchoServiceApi *pServiceApi)
{
  // set our ID
  m_bsID = bsID;
  m_GUID = lpszGUID;

  m_sRootURL = lpszRootURL;

  CString sPath(lpszPath);

  // set service API inteface
  m_ServiceApi = pServiceApi;

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
  CString appName;
  appName.Format(_T("Ancho background [%s]"), m_bsID);
  m_Magpie->Init((LPWSTR)(LPCWSTR)appName);

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
  IF_FAILED_RET(m_Magpie->AddExtension((LPWSTR)s_AnchoServiceAPIName, pServiceApi));

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
void CAnchoBackgroundAPI::OnAddonServiceReleased()
{
  m_ServiceApi.Release();
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
  if (m_BackgroundWindow) {
    return S_OK;
  }
  CStringW sURL(bsPartialURL);
  if (!GetURL(sURL)) {
    return E_FAIL;
  }

  // get the main api module and inject it into the background page
  CComPtr<IMagpieModuleRestricted> mainModule;
  IF_FAILED_RET(m_Magpie->GetModuleObject((LPOLESTR)s_AnchoMainAPIModuleID, &mainModule));

  CComPtr<IDispatch> mainModuleExports;
  IF_FAILED_RET(mainModule->GetExportsObject(&mainModuleExports));

  CComVariant chromeVT;
  IF_FAILED_RET(mainModuleExports.GetPropertyByName(s_AnchoBackgroundPageAPIName, &chromeVT));
  if (chromeVT.vt != VT_DISPATCH) {
    return E_FAIL;
  }

  CComVariant consoleVT;
  IF_FAILED_RET(mainModuleExports.GetPropertyByName(s_AnchoBackgroundConsoleObjectName, &consoleVT));
  if (consoleVT.vt != VT_DISPATCH) {
    return E_FAIL;
  }

  DispatchMap injectedObjects;
  injectedObjects[s_AnchoBackgroundPageAPIName] = chromeVT.pdispVal;
  injectedObjects[s_AnchoBackgroundConsoleObjectName] = consoleVT.pdispVal;

  IF_FAILED_RET(CBackgroundWindow::CreateBackgroundWindow(injectedObjects, sURL, &m_BackgroundWindow.p));
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoBackgroundAPI::addEventObject(BSTR aEventName, INT aInstanceId, LPDISPATCH aListener)
{
  try {
    std::wstring eventName(aEventName, SysStringLen(aEventName));

    EventObjectList &managers = m_EventObjects[eventName];
    managers.push_back(EventObjectRecord(eventName, aInstanceId, aListener));
  } catch (std::bad_alloc &) {
    ATLTRACE(L"Adding event object %s::%d failed : %s", aEventName, aInstanceId);
    return E_FAIL;
  }
  return S_OK;
}

//----------------------------------------------------------------------------
//
struct FindEventByInstanceIdFunctor
{
  FindEventByInstanceIdFunctor(int aInstanceId): instanceId(aInstanceId) {}
  int instanceId;
  bool operator()(const CAnchoBackgroundAPI::EventObjectRecord &aRec) {
    return aRec.instanceID == instanceId;
  }
};
STDMETHODIMP CAnchoBackgroundAPI::removeEventObject(BSTR aEventName, INT aInstanceId)
{
  std::wstring eventName(aEventName, SysStringLen(aEventName));
  EventObjectMap::iterator it = m_EventObjects.find(eventName);
  if (it == m_EventObjects.end()) {
    return S_OK;
  }
  it->second.remove_if(FindEventByInstanceIdFunctor(aInstanceId));
  return S_OK;
}
//----------------------------------------------------------------------------
//

STDMETHODIMP CAnchoBackgroundAPI::callFunction(LPDISPATCH aFunction, LPDISPATCH aArgs, VARIANT* aRet)
{
  ENSURE_RETVAL(aRet);
  CIDispatchHelper function(aFunction);
  VariantVector args;

  IF_FAILED_RET(addJSArrayToVariantVector(aArgs, args));
  return function.InvokeN((DISPID)0, args.size()>0? &(args[0]): NULL, args.size(), aRet);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoBackgroundAPI::invokeEventObject(BSTR aEventName, INT aSelectedInstance, BOOL aSkipInstance, LPDISPATCH aArgs, VARIANT* aRet)
{
  ENSURE_RETVAL(aRet);

  VariantVector args;
  VariantVector results;

  HRESULT hr = addJSArrayToVariantVector(aArgs, args);
  if (FAILED(hr)) {
      return hr;
  }
  hr = invokeEvent(aEventName, aSelectedInstance, aSkipInstance != FALSE, args, results);
  if (FAILED(hr)) {
      return hr;
  }
  return constructSafeArrayFromVector(results, *aRet);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoBackgroundAPI::invokeEventWithIDispatchArgument(BSTR aEventName, LPDISPATCH aArg)
{
  if (!m_InvokeEventWithIDispatch) {
    return E_FAIL;
  }
  CComVariant eventName(aEventName);
  CComVariant dispatchObject(aArg);
  return m_InvokeEventWithIDispatch.Invoke2((DISPID)0, &eventName, &dispatchObject);
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoBackgroundAPI::setIDispatchEventInvocationHandler(LPDISPATCH aFunction)
{
  m_InvokeEventWithIDispatch = aFunction;
  return S_OK;
}
//----------------------------------------------------------------------------
//

struct InvokeSelectedEventFunctor
{
  InvokeSelectedEventFunctor(VariantVector &aArgs, VariantVector &aResults, int aSelectedInstance)
    : mArgs(aArgs), mResults(aResults), mSelectedInstance(aSelectedInstance) { }
  VariantVector &mArgs;
  VariantVector &mResults;
  int mSelectedInstance;

  void operator()(CAnchoBackgroundAPI::EventObjectRecord &aRec) {
    if (aRec.instanceID == mSelectedInstance) {
      CComVariant result;
      aRec.listener.InvokeN((DISPID)0, mArgs.size()>0? &(mArgs[0]): NULL, mArgs.size(), &result);
      if (result.vt == VT_DISPATCH) {
        addJSArrayToVariantVector(result.pdispVal, mResults);
      }
    }
  }
};

struct InvokeUnselectedEventFunctor
{
  InvokeUnselectedEventFunctor(VariantVector &aArgs, VariantVector &aResults, int aSelectedInstance)
    : mArgs(aArgs), mResults(aResults), mSelectedInstance(aSelectedInstance) { }
  VariantVector &mArgs;
  VariantVector &mResults;
  int mSelectedInstance;

  void operator()(CAnchoBackgroundAPI::EventObjectRecord &aRec) {
    if (aRec.instanceID != mSelectedInstance) {
      CComVariant result;
      aRec.listener.InvokeN((DISPID)0, mArgs.size()>0? &(mArgs[0]): NULL, mArgs.size(), &result);
      if (result.vt == VT_DISPATCH) {
        addJSArrayToVariantVector(result.pdispVal, mResults);
      }
    }
  }
};

STDMETHODIMP CAnchoBackgroundAPI::invokeEvent(BSTR aEventName, INT aSelectedInstance, bool aSkipInstance, VariantVector &aArgs, VariantVector &aResults)
{
  std::wstring eventName(aEventName, SysStringLen(aEventName));
  EventObjectMap::iterator it = m_EventObjects.find(eventName);
  if (it == m_EventObjects.end()) {
    return S_OK;
  }
  if(aSkipInstance) {
    std::for_each(it->second.begin(), it->second.end(), InvokeUnselectedEventFunctor(aArgs, aResults, aSelectedInstance));
  } else {
    std::for_each(it->second.begin(), it->second.end(), InvokeSelectedEventFunctor(aArgs, aResults, aSelectedInstance));
  }
  return S_OK;
}

//----------------------------------------------------------------------------
//  _IMagpieLoggerEvents methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnLog(BSTR bsModuleID, SAFEARRAY* pVals)
{
  if (m_LogWindow)
  {
    m_LogWindow->log(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, pVals);
  }
}

//----------------------------------------------------------------------------
//
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnDebug(BSTR bsModuleID, SAFEARRAY* pVals)
{
  if (m_LogWindow)
  {
    m_LogWindow->debug(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, pVals);
  }
}

//----------------------------------------------------------------------------
//
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnInfo(BSTR bsModuleID, SAFEARRAY* pVals)
{
  if (m_LogWindow)
  {
    m_LogWindow->info(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, pVals);
  }
}

//----------------------------------------------------------------------------
//
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnWarn(BSTR bsModuleID, SAFEARRAY* pVals)
{
  if (m_LogWindow)
  {
    m_LogWindow->warn(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, pVals);
  }
}

//----------------------------------------------------------------------------
//
STDMETHODIMP_(void) CAnchoBackgroundAPI::OnError(BSTR bsModuleID, SAFEARRAY* pVals)
{
  if (m_LogWindow)
  {
    m_LogWindow->error(CComBSTR(s_BackgroundLogIdentifyer), bsModuleID, pVals);
  }
}
