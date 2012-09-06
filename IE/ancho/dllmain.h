// dllmain.h : Declaration of module class.

class CanchoModule : public CAtlDllModuleT< CanchoModule >
{
public :
  HINSTANCE m_hInstance;
	DECLARE_LIBID(LIBID_anchoLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ANCHO, "{AD78048F-6651-4A8D-B10B-4E2F65F4FD67}")
/*
	// Obtain a Class Factory
	HRESULT GetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) throw()
	{
		return AtlComModuleGetClassObject(&_AtlComModule, CLSID_AnchoRuntime, riid, ppv);
	}
*/
};

extern class CanchoModule _AtlModule;
