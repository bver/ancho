// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ancho_i.h"
#include "dllmain.h"

CanchoModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  _AtlModule.m_hInstance = hInstance;
  return _AtlModule.DllMain(dwReason, lpReserved);
}
