#pragma once

#include <stdexcept>

//TODO - create proper exception hierarchy
struct EInvalidPointer: std::exception { };

inline HRESULT exceptionToHRESULT()
{
  try {
    throw;
  } catch (EInvalidPointer&) {
    ATLTRACE("ERROR: Invalid pointer\n");
    return E_POINTER;
  } catch (std::exception &e) {
    ATLTRACE("ERROR: %s\n", e.what()); 
    return E_FAIL;
  }
}

