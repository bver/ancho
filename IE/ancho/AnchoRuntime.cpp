/****************************************************************************
 * AnchoRuntime.cpp : Implementation of CAnchoRuntime
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoRuntime.h"
#include "AnchoAddon.h"
#include "dllmain.h"


#include <Iepmapi.h>
#pragma comment(lib, "Iepmapi.lib")

extern class CanchoModule _AtlModule;

/*============================================================================
 * class CAnchoRuntime
 */

//----------------------------------------------------------------------------
//  InitAddons
HRESULT CAnchoRuntime::InitAddons()
{
  ATLASSERT(m_spUnkSite);

  BOOL val = FALSE;
  HRESULT rv = IEIsProtectedModeProcess(&val);
  if (rv == S_OK) {
    ATLTRACE(L"IE in protective mode: %s\n", val == TRUE ? L"yes" : L"no");
  }

  // get IServiceProvider to get IWebBrowser2 and IOleWindow
  CComQIPtr<IServiceProvider> pServiceProvider = m_spUnkSite;
  if (!pServiceProvider)
  {
    return E_FAIL;
  }

  // get IWebBrowser2
  pServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser.p);
  if (!m_pWebBrowser)
  {
    return E_FAIL;
  }

  // create addon service object
  IF_FAILED_RET(m_pAnchoService.CoCreateInstance(CLSID_AnchoAddonService));

  //Registering tab in service - obtains tab id and assigns it to the tab as property
  IF_FAILED_RET(m_pAnchoService->registerRuntime(this, &m_TabID));
  HWND hwnd;  m_pWebBrowser->get_HWND((long*)&hwnd);
  ::SetProp(hwnd, s_AnchoTabIDPropertyName, (HANDLE)m_TabID);

  // create all addons
  // open the registry key where all extensions are registered,
  // iterate subkeys and load each extension

  CRegKey regKey;
  LONG res = regKey.Open(HKEY_CURRENT_USER, s_AnchoExtensionsRegistryKey, KEY_READ);
  if (ERROR_SUCCESS != res)
  {
    return HRESULT_FROM_WIN32(res);
  }
  DWORD iIndex = 0;
  CString sKeyName;
  DWORD dwLen = 4096;
  HRESULT hr = S_OK;

  while(ERROR_SUCCESS == regKey.EnumKey(iIndex++, sKeyName.GetBuffer(dwLen), &dwLen, NULL))
  {
    sKeyName.ReleaseBuffer();
    CAnchoAddonComObject * pNewObject = NULL;
    hr = CAnchoAddonComObject::CreateInstance(&pNewObject);
    if (SUCCEEDED(hr))
    {
      CComPtr<IAnchoAddon> addon(pNewObject);
      hr = addon->Init(sKeyName, m_pAnchoService, m_pWebBrowser);
      if (SUCCEEDED(hr))
      {
        m_Addons[sKeyName] = addon;
      }
    }
    dwLen = 4096;
  }

  return S_OK;
}

//----------------------------------------------------------------------------
//  DestroyAddons
void CAnchoRuntime::DestroyAddons()
{
  CString sAddonID;
  POSITION pos = m_Addons.GetStartPosition();
  while(pos)
  {
    m_Addons.GetNextValue(pos)->Shutdown();
  }
  m_Addons.RemoveAll();
  if(m_pAnchoService) { 
    m_pAnchoService->unregisterRuntime(m_TabID);
  }
  m_pAnchoService.Release();
  m_pWebBrowser.Release();
}

//----------------------------------------------------------------------------
//  SetSite
STDMETHODIMP CAnchoRuntime::SetSite(IUnknown *pUnkSite)
{
  HRESULT hr = IObjectWithSiteImpl<CAnchoRuntime>::SetSite(pUnkSite);
  IF_FAILED_RET(hr);
  if (pUnkSite)
  {
    hr = InitAddons();
  }
  else
  {
    DestroyAddons();
  }
  return hr;
}

