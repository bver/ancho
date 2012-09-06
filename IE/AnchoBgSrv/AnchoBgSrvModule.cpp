/****************************************************************************
 * AnchoBgSrvModule.cpp : Implementation of CAnchoBgSrvModule
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "StdAfx.h"
#include "AnchoBgSrvModule.h"

  //----------------------------------------------------------------------------
  // Parses the command line and registers/unregisters the rgs file if necessary
bool CAnchoBgSrvModule::ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw()
{
  *pnRetCode = S_OK;
  SRVACTION action = ACTION_RUN;
  BOOL bSilent = FALSE;
  bool bRetVal = false;
  CString sAction;

  TCHAR szTokens[] = _T("-/");

  LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
  while (lpszToken != NULL)
  {
    if (WordCmpI(lpszToken, _T("s"))==0)
    {
      bSilent = TRUE;
    }

    else if (WordCmpI(lpszToken, _T("UnregServer"))==0)
    {
      action = ACTION_UNREGISTER;
    }

    else if (WordCmpI(lpszToken, _T("RegServer"))==0)
    {
      action = ACTION_REGISTER;
    }

    else if (WordCmpI(lpszToken, _T("UnregServerPerUser"))==0)
    {
      action = ACTION_UNREGISTER_PER_USER;
    }

    else if (WordCmpI(lpszToken, _T("RegServerPerUser"))==0)
    {
      action = ACTION_REGISTER_PER_USER;
    }

    lpszToken = FindOneOf(lpszToken, szTokens);
  }

  switch(action)
  {
    case ACTION_REGISTER_PER_USER:
      sAction = _T("registering per user");
      *pnRetCode = AtlSetPerUserRegistration(true);
      if (FAILED(*pnRetCode))
      {
        break;
      }
      // fallthrough
    case ACTION_REGISTER:
      sAction = _T("registering");
      *pnRetCode = RegisterAppId();
      if (SUCCEEDED(*pnRetCode))
        *pnRetCode = RegisterServer(TRUE);
      break;
    case ACTION_UNREGISTER_PER_USER:
      sAction = _T("unregistering per user");
      *pnRetCode = AtlSetPerUserRegistration(true);
      if (FAILED(*pnRetCode))
      {
        break;
      }
      // fallthrough
    case ACTION_UNREGISTER:
      sAction = _T("unregistering");
      *pnRetCode = UnregisterServer(TRUE);
      if (SUCCEEDED(*pnRetCode))
        *pnRetCode = UnregisterAppId();
      break;
    default:
      bRetVal = true;
      break;
  }
  if (!bSilent && !bRetVal)
  {
    CString sMsg;
    if (FAILED((*pnRetCode)))
    {
      CString sErr;
      ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, (*pnRetCode), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), sErr.GetBuffer(4096), 4096, NULL);
      sErr.ReleaseBuffer();
      sMsg.Format(_T("Error while %s:\n%s"), sAction, sErr);
      ::MessageBox(NULL, sMsg, _T("Ancho Background service"), MB_OK|MB_ICONERROR);
    }
    else
    {
      sMsg.Format(_T("%s succeeded."), sAction);
      ::MessageBox(NULL, sMsg, _T("Ancho Background service"), MB_OK|MB_ICONINFORMATION);
    }
  }

  return bRetVal;
}

HMODULE CAnchoBgSrvModule::GetResourceInstance()
{
  return ::GetModuleHandle(NULL);
}
