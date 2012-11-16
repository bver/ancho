#pragma once

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::SetSite(IUnknown *pUnkSite)
{
  m_spUnkSite = pUnkSite;

  if (m_spUnkSite) {
    HRESULT hr = InternalSetSite();
    ATLASSERT(hr == S_OK);
  } else {
    HRESULT hr = InternalReleaseSite();
    ATLASSERT(hr == S_OK);
  }
  return S_OK;
}

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi)
{
  if (pdbi) {
    m_dwBandID = dwBandID;
    m_dwViewMode = dwViewMode;

    const wchar_t * title;
    POINTL minSize;
    GetBandInfoValues(title, minSize);

    if (pdbi->dwMask & DBIM_MINSIZE) {
      pdbi->ptMinSize.x = minSize.x;
      pdbi->ptMinSize.y = minSize.y;
    }

    if (pdbi->dwMask & DBIM_MAXSIZE) {
      pdbi->ptMaxSize.x = -1;
      pdbi->ptMaxSize.y = -1;
    }

    if (pdbi->dwMask & DBIM_INTEGRAL) {
      pdbi->ptIntegral.x = 1;
      pdbi->ptIntegral.y = 1;
    }

    if (pdbi->dwMask & DBIM_ACTUAL) {
      pdbi->ptActual.x = 0;
      pdbi->ptActual.y = 0;
    }

    if (pdbi->dwMask & DBIM_TITLE) {
      if (!title) {
        pdbi->dwMask &= ~DBIM_TITLE;
      } else {
        wcscpy_s(pdbi->wszTitle, _countof(pdbi->wszTitle), title);
      }
    }

    if (pdbi->dwMask & DBIM_MODEFLAGS) {
      pdbi->dwModeFlags = DBIMF_VARIABLEHEIGHT;
    }

    if (pdbi->dwMask & DBIM_BKCOLOR) {
      pdbi->dwMask &= ~DBIM_BKCOLOR;
    }
    return S_OK;
  }
  return E_INVALIDARG;
}

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::GetWindow(HWND* phwnd)
{
  if (!phwnd) {
    return E_POINTER;
  }
  *phwnd = mContentWindow->operator HWND();
  return S_OK;
}

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::ContextSensitiveHelp(BOOL fEnterMode)
{
  return S_OK;
}

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::CloseDW(unsigned long dwReserved)
{
  return ShowDW(FALSE);
}

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::ResizeBorderDW(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved)
{
  return E_NOTIMPL;
}

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::ShowDW(BOOL fShow)
{
  mContentWindow->ShowWindow(fShow ? SW_SHOW : SW_HIDE);
  return S_OK;
}

/*template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::HasFocusIO(void)
{
  HWND hFocusWnd = ::GetFocus();
  return ::IsChild((HWND)*mContentWindow, hFocusWnd) ? S_OK : S_FALSE;
}*/

/*template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::TranslateAcceleratorIO(LPMSG lpMsg)
{
  CComQIPtr<IOleInPlaceActiveObject, &IID_IOleInPlaceActiveObject> pIOIPAO(mContentWindow->m_pWebBrowser);
  if (!pIOIPAO || pIOIPAO->TranslateAccelerator(lpMsg) != S_OK) {
    TranslateMessage(lpMsg);
    DispatchMessage(lpMsg);
  }

  return S_OK;
}*/

/*template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
STDMETHODIMP CBasicToolbar<TImplementation, TContentWindow, tClsid>::UIActivateIO(BOOL fActivate, LPMSG lpMsg)
{
  if (fActivate) {
    mContentWindow->SetFocus();
  }
  return S_OK;
}*/

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
HRESULT CBasicToolbar<TImplementation, TContentWindow, tClsid>::InternalSetSite()
{
  // Get the parent window.
  IOleWindow *pOleWindow = NULL;
  mHWNDParent = NULL;

  if (SUCCEEDED(m_spUnkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&pOleWindow))) {
    pOleWindow->GetWindow(&mHWNDParent);
    pOleWindow->Release();
  }

  if (!::IsWindow(mHWNDParent)) {
    return E_FAIL;
  }

  // Get the IInputObjectSite so we can signal when we get keyboard focus.
  mInputObjectSite = m_spUnkSite;

  return S_OK;
}

template<typename TImplementation, typename TContentWindow, const CLSID* tClsid>
HRESULT CBasicToolbar<TImplementation, TContentWindow, tClsid>::InternalReleaseSite()
{
  if (mContentWindow->IsWindow())
  {
    mContentWindow->DestroyWindow();
  }
  return S_OK;
}