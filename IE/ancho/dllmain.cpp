// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ancho_i.h"
#include "dllmain.h"

#include "ProtocolCF.h"
#include "AnchoPassthruAPP.h"

CanchoModule _AtlModule;
CComPtr<IClassFactory> _CFHTTP;
CComPtr<IClassFactory> _CFHTTPS;

typedef PassthroughAPP::CMetaFactory<PassthroughAPP::CComClassFactoryProtocol,
  CAnchoPassthruAPP> MetaFactory;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  _AtlModule.m_hInstance = hInstance;
  if (dwReason == DLL_PROCESS_ATTACH) {
    // Register the Passthrough APP
    CComPtr<IInternetSession> pInternetSession;
    if (FAILED(CoInternetGetSession(0, &pInternetSession, 0))) {
      return FALSE;
    }

    if (FAILED(MetaFactory::CreateInstance(CLSID_HttpProtocol, &_CFHTTP))) {
      return FALSE;
    }
    if (FAILED(pInternetSession->RegisterNameSpace(_CFHTTP, CLSID_NULL, L"http", 0, 0, 0))) {
      return FALSE;
    }
    if (FAILED(MetaFactory::CreateInstance(CLSID_HttpSProtocol, &_CFHTTPS))) {
      return FALSE;
    }
    if (FAILED(pInternetSession->RegisterNameSpace(_CFHTTPS, CLSID_NULL, L"https", 0, 0, 0))) {
      return FALSE;
    }
  }
  else if (dwReason == DLL_PROCESS_DETACH) {
    CComPtr<IInternetSession> pInternetSession;
    if (FAILED(CoInternetGetSession(0, &pInternetSession, 0))) {
      return FALSE;
    }
    pInternetSession->UnregisterNameSpace(_CFHTTP, L"http");
    pInternetSession->UnregisterNameSpace(_CFHTTPS, L"https");
  }

  return _AtlModule.DllMain(dwReason, lpReserved);
}
