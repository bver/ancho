#include "stdafx.h"
#include "Toolbar.h"

void CToolbar::GetBandInfoValues(const wchar_t *& title, POINTL &minSize)
{
  title = L"TEST_TOOLBAR";
  minSize.x = 200;
  minSize.y = 35;
}

HRESULT CToolbar::InternalSetSite()
{
  /*if (!ResolveResourcesDir(m_ResourcesDir))
  {
    return E_FAIL;
  }*/

  // Get the parent window.
  IOleWindow *pOleWindow = NULL;
  m_hWndParent = NULL;

  if (SUCCEEDED(m_spUnkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&pOleWindow)))
  {
    pOleWindow->GetWindow(&m_hWndParent);
    pOleWindow->Release();
  }

  if (!::IsWindow(m_hWndParent))
  {
    return E_FAIL;
  }

  // Get the IInputObjectSite so we can signal when we get keyboard focus.
  m_InputObjectSite = m_spUnkSite;

  return RunToolbarPage();
}

HRESULT CToolbar::InternalReleaseSite()
{
  m_HtmlWindow->m_External.Release();
  if (m_HtmlWindow->IsWindow())
  {
    m_HtmlWindow->DestroyWindow();
  }

  //CleanupScriptedClient();

  return S_OK;
}

/*void CToolbar::GetToolbarPage(std::wstring &pageUrl)
{
  pageUrl = L"salsita://allow/html/ie-toolbar.html";
}*/

/*const IID *CToolbar::GetBackgroundScriptServiceFactoryCLSID() const
{
  return &AllowServiceLib::CLSID_AllowServiceFactory;
}*/

/*bool CToolbar::ResolveResourcesDir(std::wstring &result)
{
  return CSalsitaExtensionHelper::ResourcesDirReadFromRegistry(HKEY_LOCAL_MACHINE, CONFIG_SUBKEY, CONFIG_RESDIR, result);
}*/

/*HRESULT CToolbar::ExecuteContentScript()
{
  IF_FAILED_RET(m_Magpie->Run(L"js/ie-toolbar.js"));
  return S_OK;
}*/