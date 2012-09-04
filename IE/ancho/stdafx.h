// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include "resource.h"
#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atlctl.h>

#include <shlguid.h>
#include <exdispid.h>

using namespace ATL;

// Magpie
#ifdef _DEBUG
# import "..\Magpie\Debug\Magpie.dll" named_guids raw_interfaces_only raw_native_types no_smart_pointers
#else
# import "..\Magpie\Release\Magpie.dll" named_guids raw_interfaces_only raw_native_types no_smart_pointers
#endif
using namespace MagpieLib;
#include "CreateMagpieInstance.h"

// addon framework
#ifdef _DEBUG
# import "..\Debug\AnchoBgSrv.exe" named_guids no_smart_pointers raw_interfaces_only raw_native_types no_auto_exclude
#else
# import "..\Release\AnchoBgSrv.exe" named_guids no_smart_pointers raw_interfaces_only raw_native_types no_auto_exclude
#endif
using namespace AnchoBgSrvLib;

#define _DEBUG_BREAK
#ifdef _DEBUG_BREAK
#define ASSERT_ ATLASSERT
#else
#define ASSERT_
#endif

#define IF_FAILED_RET(_hr) \
  do \
  { \
    HRESULT _hr__ = _hr; \
    ASSERT_(SUCCEEDED(_hr__)); \
    if (FAILED(_hr__)) \
    { \
      ATLTRACE(_T("ASSERTION FAILED: 0x%08x in "), _hr__); \
      ATLTRACE(__FILE__); \
      ATLTRACE(_T(" line %i\n"), __LINE__); \
      return _hr__; \
    } \
  } while(0);

#define IF_FAILED_RET2(_hr, _ret) \
  do \
  { \
    HRESULT _hr__ = _hr; \
    ASSERT_(SUCCEEDED(_hr__)); \
    if (FAILED(_hr__)) \
    { \
      ATLTRACE(_T("ASSERTION FAILED: 0x%08x in "), _hr__); \
      ATLTRACE(__FILE__); \
      ATLTRACE(_T(" line %i\n"), __LINE__); \
      return _ret; \
    } \
  } while(0);

#define IF_FAILED_BREAK(_hr, _hrRet) \
    _hrRet = _hr; \
    ASSERT_(SUCCEEDED(_hrRet)); \
    if (FAILED(_hrRet)) \
    { \
      ATLTRACE(_T("ASSERTION FAILED: 0x%08x in "), _hrRet); \
      ATLTRACE(__FILE__); \
      ATLTRACE(_T(" line %i\n"), __LINE__); \
      break; \
    }

#define ENSURE_RETVAL(_val) \
  if (!_val) return E_POINTER;
