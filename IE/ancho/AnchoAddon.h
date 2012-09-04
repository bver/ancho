// AnchoAddon.h : Declaration of the CAnchoAddon

#pragma once
#include "resource.h"       // main symbols

#include "ancho_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CAnchoAddon
class CAnchoAddon;
typedef IDispEventImpl<1, CAnchoAddon, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0> DWebBrowserEvents2Ancho;

class ATL_NO_VTABLE CAnchoAddon :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAnchoAddon, &CLSID_AnchoAddon>,
	public IObjectWithSiteImpl<CAnchoAddon>,
  public DWebBrowserEvents2Ancho,
	public IAnchoAddon
{
public:
  CAnchoAddon() : m_InstanceID(0), m_dwAdviseSinkWebBrowser(0)
	{
	}

  DECLARE_REGISTRY_RESOURCEID(IDR_ANCHOADDON)
  DECLARE_NOT_AGGREGATABLE(CAnchoAddon)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(CAnchoAddon)
	  COM_INTERFACE_ENTRY(IAnchoAddon)
	  COM_INTERFACE_ENTRY(IObjectWithSite)
  END_COM_MAP()

  BEGIN_SINK_MAP(CAnchoAddon)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, BrowserNavigateCompleteEvent)
  END_SINK_MAP()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
    DestroyAddon();
	}

public:
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
  STDMETHOD_(void, BrowserNavigateCompleteEvent)(IDispatch *pDisp, VARIANT *URL);

private:
  HRESULT InitAddon();
  void DestroyAddon();

private:
  static LPCWSTR                      s_ExtensionName;
  DWORD                               m_dwAdviseSinkWebBrowser;     // Cookie for AtlAdvise
  ULONG                               m_InstanceID;
  CComQIPtr<IWebBrowser2>             m_pWebBrowser;

  CComPtr<IAnchoAddonService>           m_pAnchoService;
  CComPtr<IAnchoAddonBackground>        m_pAddonBackground;
  CComQIPtr<IAnchoBackgroundConsole>    m_pBackgroundConsole;
  CComQIPtr<IDispatch>                m_pContentAPI;

  CComPtr<IMagpieApplication>         m_Magpie;

};

OBJECT_ENTRY_AUTO(__uuidof(AnchoAddon), CAnchoAddon)
