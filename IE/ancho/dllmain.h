// dllmain.h : Declaration of module class.

class CAnchoModule : public CAtlDllModuleT<CAnchoModule>
{
public :
  HINSTANCE m_hInstance;
  DECLARE_LIBID(LIBID_anchoLib)
  DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ANCHO, "{AD78048F-6651-4A8D-B10B-4E2F65F4FD67}")

  BOOL WINAPI DllMain(DWORD dwReason, LPVOID lpReserved) throw();

protected:
  CComPtr<IClassFactory> m_CFHTTP;
  CComPtr<IClassFactory> m_CFHTTPS;
};

extern class CAnchoModule _AtlModule;
