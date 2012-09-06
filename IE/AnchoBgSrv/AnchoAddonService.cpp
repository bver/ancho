/****************************************************************************
 * AnchoAddonService.cpp : Implementation of CAnchoAddonService
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "AnchoAddonService.h"

/*============================================================================
 * class CAnchoAddonBackground
 */

//----------------------------------------------------------------------------
//
void CAnchoAddonService::OnAddonFinalRelease(BSTR bsID)
{
  m_Objects.RemoveKey(bsID);
}

//----------------------------------------------------------------------------
//
HRESULT CAnchoAddonService::FinalConstruct()
{
  // Get and store the path, this will be used in some places (e.g. to load
  // additional DLLs).
  LPTSTR psc = m_sThisPath.GetBuffer(_MAX_PATH);
  GetModuleFileName(_AtlModule.m_hInstance, psc, _MAX_PATH);
  PathRemoveFileSpec(psc);
  PathAddBackslash(psc);
  m_sThisPath.ReleaseBuffer();
  return S_OK;
}

//----------------------------------------------------------------------------
//
void CAnchoAddonService::FinalRelease()
{
  CAnchoAddonBackgroundComObject* pObject;
  CString sID;
  POSITION pos = m_Objects.GetStartPosition();
  while(pos)
  {
    m_Objects.GetNextAssoc(pos, sID, pObject);
    pObject->AddonServiceLost();
  }
  m_Objects.RemoveAll();
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::GetExtension(BSTR bsID, IAnchoAddonBackground ** ppRet)
{
  ENSURE_RETVAL(ppRet);

  CAnchoAddonBackgroundComObject* pObject = NULL;
  CComPtr<IAnchoAddonBackground> ptr;

  if (!m_Objects.Lookup(bsID, pObject))
  {
    // not found, create new instance
    ATLTRACE(_T("ADD OBJECT %s\n"), bsID);
    HRESULT hr = CAnchoAddonBackgroundComObject::CreateInstance(&pObject);
    if (FAILED(hr))
    {
      return hr;
    }
    // store to a safepointer
    ptr = pObject;

    ///////////////////////////////////////////////////////
    // Init the whole addon. This will load and init the
    // Ancho api.
    hr = pObject->Init(m_sThisPath, this, bsID);
    if (FAILED(hr))
    {
      return hr;
    }
    // store in map
    m_Objects[bsID] = pObject;
  }
  else
  {
    ATLTRACE(_T("FOUND OBJECT %s\n"), bsID);
    // found, create a new intance ID
    // store to safepointer
    ptr = pObject;
  }

  // set return value
  (*ppRet) = ptr.Detach();
  return S_OK;
}

//----------------------------------------------------------------------------
//
STDMETHODIMP CAnchoAddonService::GetModulePath(BSTR * pbsPath)
{
  ENSURE_RETVAL(pbsPath);
  (*pbsPath) = m_sThisPath.AllocSysString();
  return S_OK;
}
