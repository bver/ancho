#pragma once

//#include "errorchecking.h"

class CHtmlPanelUIHandlerBase
{
protected:
  HACCEL m_blockedAcceleratorsTable;

  CHtmlPanelUIHandlerBase();
  virtual ~CHtmlPanelUIHandlerBase();
};

template <class Timpl>
class CHtmlPanelUIHandler :
  public CHtmlPanelUIHandlerBase,
  public IDispatchImpl<IDocHostUIHandlerDispatch, &IID_IDocHostUIHandlerDispatch, &LIBID_MSHTML>
{
public:
  CComPtr<IDispatch>  m_External;
  STDMETHOD(ShowContextMenu)(DWORD dwID, DWORD x, DWORD y, IUnknown *pcmdtReserved, IDispatch *pdispReserved, HRESULT *dwRetVal)
  {
    if (dwID != CONTEXT_MENU_CONTROL) {
      ENSURE_RETVAL(dwRetVal);
      *dwRetVal = S_OK;
    }

    return S_OK;
  }

  /**
   * Modifies the webbrowser behaviour.
   * Sets content not selectable, IE theme support, postpone script execution, no border.
   */
  STDMETHOD(GetHostInfo)(DWORD *pdwFlags, DWORD *pdwDoubleClick)
  {
    ENSURE_RETVAL(pdwFlags);
    ENSURE_RETVAL(pdwDoubleClick);
    *pdwFlags = DOCHOSTUIFLAG_DIALOG | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_NO3DOUTERBORDER;
    *pdwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
    return S_OK;
  }

  STDMETHOD(ShowUI)(DWORD dwID, IUnknown *pActiveObject, IUnknown *pCommandTarget, IUnknown *pFrame, IUnknown *pDoc, HRESULT *dwRetVal)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(HideUI)(void)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(UpdateUI)(void)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(EnableModeless)(VARIANT_BOOL fEnable)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(OnDocWindowActivate)(VARIANT_BOOL fActivate)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(OnFrameWindowActivate)(VARIANT_BOOL fActivate)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(ResizeBorder)(long left, long top, long right, long bottom, IUnknown *pUIWindow, VARIANT_BOOL fFrameWindow)
  {
    return E_NOTIMPL;
  }

  /**
   * Filters out some navigation keys by filtering accelerators defined in SalsitaToolbarBrowserCustomization.cpp as blockedAccelerators.
   */
  STDMETHOD(TranslateAccelerator)(DWORD hWnd, DWORD nMessage, DWORD wParam, DWORD lParam, BSTR bstrGuidCmdGroup, DWORD nCmdID, HRESULT *dwRetVal)
  {
    ENSURE_RETVAL(dwRetVal);
    *dwRetVal = S_FALSE;
    MSG msg = { (HWND)hWnd, nMessage, wParam, lParam, 0, { 0, 0 } };
    if (::TranslateAccelerator((HWND)hWnd, m_blockedAcceleratorsTable, &msg))
    {
      *dwRetVal = S_OK; ///< block this accelerator
    }
    return S_OK;
  }

  STDMETHOD(GetOptionKeyPath)(BSTR *pbstrKey, DWORD dw)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(GetDropTarget)(IUnknown *pDropTarget, IUnknown **ppDropTarget)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(GetExternal)(IDispatch **ppDispatch)
  {
    ENSURE_RETVAL(ppDispatch);
    if (m_External)
    {
      return m_External.QueryInterface(ppDispatch);
    }
    *ppDispatch = NULL;
    return S_FALSE;
  }

  STDMETHOD(TranslateUrl)(DWORD dwTranslate, BSTR bstrURLIn, BSTR *pbstrURLOut)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(FilterDataObject)(IUnknown *pDO, IUnknown **ppDORet)
  {
    return E_NOTIMPL;
  }
};