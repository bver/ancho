/****************************************************************************
 * IECookieManager.cpp : Implementation of CIECookieManager
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "StdAfx.h"
#include "IECookieManager.h"
#include <set>
#include <Shlobj.h>


CString lastErrorMessage(const DWORD& dwErrorCode)
{
	LPTSTR lpErrorText = NULL;
 
	::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
		0, dwErrorCode, 0, lpErrorText, MAX_PATH, 0 );
 
  CString msg = lpErrorText;
	::LocalFree( lpErrorText );
  return msg;
}


CIECookieManager::ParseBuffer::ParseBuffer(size_t size) :
  m_Size(size)
{
  m_Buffer = new char[size];
}
CIECookieManager::ParseBuffer::ParseBuffer(size_t size, char * buffer) :
  m_Size(size)
{
  m_Buffer = new char[size];
  memcpy(m_Buffer, buffer, size);
}

CIECookieManager::ParseBuffer::~ParseBuffer()
{
  delete [] m_Buffer;
}

char & CIECookieManager::ParseBuffer::operator [](size_t pos)
{
  return m_Buffer[pos];
}

CIECookieManager::ParseBuffer::operator char * ()
{
  return m_Buffer;
}

CIECookieManager::ParseBuffer::operator void * ()
{
  return m_Buffer;
}

HRESULT CIECookieManager::GetCookieFolder(CString& sPath) {
  HRESULT hr = SHGetFolderPath(NULL, CSIDL_COOKIES, NULL, SHGFP_TYPE_CURRENT, sPath.GetBuffer(MAX_PATH));
  sPath.ReleaseBuffer();
  IF_FAILED_RET(hr);
  sPath += _T('\\');
  return S_OK;
}
HRESULT CIECookieManager::ParseCookies(ParseBuffer & parseBuffer, size_t sLen, CCookieArray & cookies) {
  // parse cookies
  char * pStart = parseBuffer;
  CookieParsedData cookieData;
  for(size_t n = 0; n < sLen; n++)
  {
    switch(parseBuffer[n])
    {
      case '\n':
      case '\r':
        parseBuffer[n] = 0;
        if (!cookieData.Add(pStart))
        {
          // make sure we have a delimiter
          if (*pStart != '*')
          {
            return E_FAIL;
          }
          // add cookie
          CComPtr<CIECookieComObject> pCookie;
          IF_FAILED_RET(CIECookie::CreateObject(cookieData, pCookie.p));
          cookies.push_back(pCookie);
          cookieData.Reset();
        }
        pStart = &parseBuffer[n+1];
    }
  }

  return S_OK;
}

// try to read cookie content from given url
HRESULT CIECookieManager::ReadCookieFile(LPCWSTR url, CCookieArray & cookies) {
  LPINTERNET_CACHE_ENTRY_INFO info;
  DWORD MAX_CACHE_ENTRY_INFO_SIZE = 10 * 4096;
  info = (LPINTERNET_CACHE_ENTRY_INFO)malloc(MAX_CACHE_ENTRY_INFO_SIZE);
  HANDLE streamHandle;
  streamHandle = RetrieveUrlCacheEntryStream(url, info, &MAX_CACHE_ENTRY_INFO_SIZE, false, 0);
  HRESULT hr;
  if (streamHandle == NULL) {
    hr = GetLastError();
    ATLTRACE(_T("Cannot open cache entry stream. Error: %d; url: %ws\n"), hr, url);
  } else {
    DWORD dwStreamSize = info->dwSizeLow;
    char data[4096];
    if (!ReadUrlCacheEntryStream(streamHandle, 0, &data, &dwStreamSize, 0)) {
      hr = GetLastError();
      ATLTRACE(_T("Cannot read cache entry stream. Error: %d\n"), hr);
    } else {
      UnlockUrlCacheEntryStream(streamHandle, 0);
      data[dwStreamSize] = '\0';
      ParseBuffer parserBuffer(dwStreamSize, data);
      hr = ParseCookies(parserBuffer, dwStreamSize, cookies);
    }
  }
  delete info;
  return hr;
}


void CIECookieManager::FileWatcher(void* args) {
  CoInitializeEx(NULL, COINIT_MULTITHREADED);
  CIECookieManager* cookieManager = (CIECookieManager*)args;
  CString sPath;
  HRESULT hr = GetCookieFolder(sPath);
  ASSERT_(SUCCEEDED(hr));
  if (FAILED(hr)) {
     ATLTRACE(_T("ASSERTION FAILED: 0x%08x in "), hr);
     ATLTRACE(__FILE__);
     ATLTRACE(_T(" line %i\n"), __LINE__);
     return;
  }

  HANDLE hDir = CreateFile( sPath, // pointer to the file name
    FILE_LIST_DIRECTORY,                // access (read/write) mode
    FILE_SHARE_READ | FILE_SHARE_DELETE,// share mode
    NULL,                               // security descriptor
    OPEN_EXISTING,                      // how to create
    FILE_FLAG_BACKUP_SEMANTICS,         // file attributes
    NULL                                // file with attributes to copy
  );

  FILE_NOTIFY_INFORMATION Buffer[1024];
  PFILE_NOTIFY_INFORMATION pInfo;
  DWORD BytesReturned;
  while( ReadDirectoryChangesW(
    hDir,                               // handle to directory
    &Buffer,                            // read results buffer
    sizeof(Buffer),                     // length of buffer
    TRUE,                               // monitoring option
    FILE_NOTIFY_CHANGE_SECURITY|
    FILE_NOTIFY_CHANGE_CREATION|
    FILE_NOTIFY_CHANGE_LAST_ACCESS|
    FILE_NOTIFY_CHANGE_LAST_WRITE|
    FILE_NOTIFY_CHANGE_SIZE|
    FILE_NOTIFY_CHANGE_ATTRIBUTES|
    FILE_NOTIFY_CHANGE_DIR_NAME|
    FILE_NOTIFY_CHANGE_FILE_NAME|
    0,                                  // filter conditions
    &BytesReturned,                     // bytes returned
    NULL,                               // overlapped buffer
    NULL                                // completion routine
    ))
  {
    int i=0;
    int nextOffset = 0;
    std::set<CString> added;

    do {
      i = nextOffset;
      pInfo = (PFILE_NOTIFY_INFORMATION)(((BYTE*)&Buffer) + nextOffset);
      CString fn = CString(pInfo->FileName).Left(pInfo->FileNameLength / sizeof(wchar_t));
      CString fileName = sPath + fn;
      switch(pInfo->Action) {
        case FILE_ACTION_ADDED: added.insert(fileName); break;
        case FILE_ACTION_REMOVED: added.erase(fileName); break;
        case FILE_ACTION_MODIFIED: added.insert(fileName); break;
        case FILE_ACTION_RENAMED_OLD_NAME: break;
        case FILE_ACTION_RENAMED_NEW_NAME: break;
      }
      nextOffset += pInfo->NextEntryOffset;
    } while (pInfo->NextEntryOffset != 0);

    std::set<CString>::iterator it;
    for (it = added.begin(); it != added.end(); it++) {
      cookieManager->OnCookieFileChanged(*it);
    }
  }
}
/*============================================================================
 * class CIECookieManager
 */

//----------------------------------------------------------------------------
//  CTOR
CIECookieManager::CIECookieManager()
{
  //m_pOnCookiesChangedCallback = NULL;
  HWND hwnd = m_CookieHelperWindow.Create(NULL);
  m_CookieHelperWindow.m_IECookieManager = this;
  HRESULT hr = GetLastError();
  ATLASSERT(hwnd);
}

//----------------------------------------------------------------------------
//  FinalConstruct
HRESULT CIECookieManager::FinalConstruct()
{
  return S_OK;
}

//----------------------------------------------------------------------------
//  FinalRelease
void CIECookieManager::FinalRelease()
{
  m_CookieHelperWindow.DestroyWindow();
}

HRESULT CIECookieManager::ParseCookieFile(LPCTSTR lpszFilename, CCookieArray & cookies)
{
  CAtlFile f;
  HRESULT hr = f.Create(lpszFilename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
  // this usually fails when the cookie is changed twice in a short time and filewatcher
  // recognizes file delete in following bulk of file changes. The cookie storage file is
  // recreated after each change of cookie
  if (FAILED(hr)) {
    return hr;
  }

  ULONGLONG nLen = 0;
  IF_FAILED_RET(f.GetSize(nLen));
  // limit script size to 4GB
  if (nLen > 0x00000000ffffffff)
  {
    return E_OUTOFMEMORY;
  }

  size_t sLen = (size_t)(nLen & 0x00000000ffffffff);
  ParseBuffer buffer(sLen);
  IF_FAILED_RET(f.Read(buffer, sLen));
  hr = ParseCookies(buffer, sLen, cookies);
  return hr;
}

STDMETHODIMP CIECookieManager::enumCookies(LPDISPATCH pCallback)
{
  if (!pCallback)
  {
    return E_INVALIDARG;
  }

  DWORD dwEntrySize;
  DWORD hr = S_OK;
  LPCTSTR filter = _T("cookie:");
  LPINTERNET_CACHE_ENTRY_INFO entry;
  DWORD MAX_CACHE_ENTRY_INFO_SIZE = 4096;
  HANDLE hCacheDir;
  int nCount = 0;
  BOOL ok = FALSE;
  dwEntrySize = MAX_CACHE_ENTRY_INFO_SIZE;
  entry = (LPINTERNET_CACHE_ENTRY_INFO)malloc(MAX_CACHE_ENTRY_INFO_SIZE);
  hCacheDir = FindFirstUrlCacheEntry(filter, entry, &dwEntrySize);
  if (hCacheDir) {
    CComVariant cookieVT;
    DISPPARAMS params = {&cookieVT, NULL, 1, 0};
    CIDispatchHelper callback(pCallback);
    do {
      CCookieArray cookies;
      dwEntrySize = MAX_CACHE_ENTRY_INFO_SIZE;
      ReadCookieFile(entry->lpszSourceUrlName, cookies);
      for(size_t n = 0; n < cookies.size(); ++n) {
        cookieVT = cookies[n];
        callback.Call(NULL, &params);
        nCount++;
      }
      ok = FindNextUrlCacheEntry(hCacheDir, entry, &dwEntrySize);
    } while (ok);
    FindCloseUrlCache(hCacheDir);
  } else {
    hr = GetLastError();
  }
  delete entry;
  return hr;
}
HRESULT CIECookieManager::OnCookieFileChanged(LPCTSTR lpszFileName) {
  // continue with cookie change event
  if (m_OnCookiesChangedCallback.get()/*m_pOnCookiesChangedCallback != NULL*/) {
    CCookieArray cookies;
    HRESULT hr = ParseCookieFile(lpszFileName, cookies);
    if (S_OK != hr ) {
      return hr;
    }
    m_CookieHelperWindow.SendMessageW(WM_ONCOOKIEMESSAGE, (WPARAM)&cookies);
  }
  return S_OK;
}
//STDMETHODIMP CIECookieManager::put_cookiesChangedCallback(LPDISPATCH apOnCookiesChangedCallback) 
void CIECookieManager::startWatching()
{
  _beginthread(CIECookieManager::FileWatcher, 0,this);
  // file watcher must be started only from background script, otherwise the callback gets garbage collected
  /*BOOL startFileWatcher = !m_pOnCookiesChangedCallback && apOnCookiesChangedCallback != NULL;
  m_pOnCookiesChangedCallback = apOnCookiesChangedCallback;
  if (startFileWatcher) {
    _beginthread(CIECookieManager::FileWatcher, 0,this);
  }
  return S_OK;*/
}

STDMETHODIMP CIECookieManager::removeCookie(BSTR aUrl, BSTR aName) {
  CString url = CString(aUrl);
  // set Expires attribute to old time removes given cookie
  CString value = CString(aName) + CString("; Expires=Thu, 01 Jan 1970 00:00:01 GMT");
  if (!InternetSetCookie(url, NULL, value)) {
    HRESULT hr = GetLastError();
    ATLTRACE(_T("Could not remove cookie. Error: %d\n"), hr);
    return hr;
  }
  return S_OK;
}

STDMETHODIMP CIECookieManager::setCookie(BSTR aUrl, BSTR aName, BSTR aData)
{
  if (!InternetSetCookie(aUrl, aName, aData)) {
    HRESULT hr = GetLastError();
    ATLTRACE(_T("Could not set cookie. Error: %d : %s\n"), hr, lastErrorMessage(hr));
    return hr;
  }
  return S_OK;
}

STDMETHODIMP CIECookieManager::getCookie(BSTR aUrl, BSTR aName, VARIANT *aData)
{
  ENSURE_RETVAL(aData);
  DWORD size = 0;
  if (!InternetGetCookie(aUrl, aName, NULL, &size)){
    HRESULT hr = GetLastError();
    ATLTRACE(_T("Could not set cookie. Error: %d : %s\n"), hr, lastErrorMessage(hr));
    return hr;
  }
  CString data;
  //Size was returned in bytes not in number of characters.
  TCHAR * buffer = data.GetBuffer(size/sizeof(TCHAR) + 1);
  if (!InternetGetCookie(aUrl, aName, buffer, &size)){
    data.ReleaseBuffer();
    HRESULT hr = GetLastError();
    ATLTRACE(_T("Could not set cookie. Error: %d : %s\n"), hr, lastErrorMessage(hr));
    SysFreeString(aData->bstrVal);
    return hr;
  }
  data.ReleaseBuffer();
  aData->vt = VT_BSTR;
  aData->bstrVal = data.AllocSysString();

  //TODO construct IECookie object from string
  return S_OK;
}

LRESULT CIECookieManager::CCookieHelperWindow::OnCookieMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
  ATLASSERT(m_IECookieManager);
  ATLASSERT(m_IECookieManager->m_OnCookiesChangedCallback.get());
  CCookieArray * cookies = (CCookieArray *)wParam;
  CComVariant cookieVT;
  //DISPPARAMS params = {&cookieVT, NULL, 1, 0};
  //CIDispatchHelper callback(m_IECookieManager->m_pOnCookiesChangedCallback.p);
  for(size_t i = 0; i < cookies->size(); ++i)
  {
    cookieVT = (*cookies)[i];
    (m_IECookieManager->m_OnCookiesChangedCallback)->operator ()(cookieVT);
    ATLTRACE(L"ANCHO COOKIE MANAGER CALLBACK\n");
    //HRESULT hr = callback.Call(NULL, &params);
    //ATLASSERT(hr == 0);
  }
  return 0;
}