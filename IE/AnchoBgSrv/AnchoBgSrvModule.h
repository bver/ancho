/****************************************************************************
 * AnchoBgSrvModule.h : Declaration of CAnchoBgSrvModule
 * Copyright 2012 Salsita software (http://www.salsitasoft.com).
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once
#include "AnchoBgSrv_i.h"

/*============================================================================
 * class CAnchoBgSrvModule
 */
class CAnchoBgSrvModule : public CAtlExeModuleT< CAnchoBgSrvModule >
{
public :
  HINSTANCE m_hInstance;
	DECLARE_LIBID(LIBID_AnchoBgSrvLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ANCHOBGSRV, "{FBA7ED0C-1181-476A-AEDE-F0AF49EF80F7}")

  //----------------------------------------------------------------------------
  // Parses the command line and registers/unregisters the rgs file if necessary
	bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw();
  HMODULE GetResourceInstance();

private:
  typedef enum
  {
    ACTION_RUN = 0,
    ACTION_REGISTER,
    ACTION_UNREGISTER,
    ACTION_REGISTER_PER_USER,
    ACTION_UNREGISTER_PER_USER
  } SRVACTION;
};

extern CAnchoBgSrvModule _AtlModule;
