// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ancho_i.h"
#include "dllmain.h"

#include "ProtocolCF.h"
#include "AnchoPassthruAPP.h"

typedef PassthroughAPP::CMetaFactory<PassthroughAPP::CComClassFactoryProtocol,
  CAnchoPassthruAPP> MetaFactory;

BOOL WINAPI CAnchoModule::DllMain(DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    // Register the Passthrough APP
    CComPtr<IInternetSession> pInternetSession;
    if (SUCCEEDED(CoInternetGetSession(0, &pInternetSession, 0))) {
      if (SUCCEEDED(MetaFactory::CreateInstance(CLSID_HttpProtocol, &m_CFHTTP))) {
        if (FAILED(pInternetSession->RegisterNameSpace(m_CFHTTP, CLSID_NULL, L"http", 0, 0, 0))) {
          // TODO: log this failure
        }
      }
      if (SUCCEEDED(MetaFactory::CreateInstance(CLSID_HttpSProtocol, &m_CFHTTPS))) {
        if (FAILED(pInternetSession->RegisterNameSpace(m_CFHTTPS, CLSID_NULL, L"https", 0, 0, 0))) {
          // TODO: log this failure
        }
      }
    }
  }
  else if (dwReason == DLL_PROCESS_DETACH) {
    CComPtr<IInternetSession> pInternetSession;
    if (SUCCEEDED(CoInternetGetSession(0, &pInternetSession, 0))) {
      pInternetSession->UnregisterNameSpace(m_CFHTTP, L"http");
      pInternetSession->UnregisterNameSpace(m_CFHTTPS, L"https");
    }
  }

  return CAtlDllModuleT<CAnchoModule>::DllMain(dwReason, lpReserved);
}

CAnchoModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  _AtlModule.m_hInstance = hInstance;
  return _AtlModule.DllMain(dwReason, lpReserved);
}
