#include "stdafx.h"
#include "HtmlPanelUIHandler.h"

static ACCEL blockedAccelerators [] = {
  { FVIRTKEY | FCONTROL | FNOINVERT, 'T', 0 },
  { FVIRTKEY | FCONTROL | FNOINVERT, 'K', 0 },
  { FVIRTKEY | FCONTROL | FNOINVERT, 'O', 0 },
  { FVIRTKEY | FCONTROL | FNOINVERT, 'S', 0 },
  { FVIRTKEY | FCONTROL | FNOINVERT, 'W', 0 },
  { FVIRTKEY | FCONTROL | FNOINVERT, 'F', 0 },
  { FVIRTKEY | FNOINVERT, VK_BROWSER_BACK, 0 },
  { FVIRTKEY | FALT | FNOINVERT, VK_LEFT, 0 },
  { FVIRTKEY | FALT | FNOINVERT, VK_RIGHT, 0 },
  { FVIRTKEY | FCONTROL | FNOINVERT, 'N', 0 },
  { FVIRTKEY | FNOINVERT, VK_F5, 0 },
  { FVIRTKEY | FNOINVERT, VK_BROWSER_FAVORITES, 0 },
  { FVIRTKEY | FNOINVERT, VK_BROWSER_FORWARD, 0 },
  { FVIRTKEY | FNOINVERT, VK_BROWSER_HOME, 0 },
  { FVIRTKEY | FNOINVERT, VK_BROWSER_REFRESH, 0 },
  { FVIRTKEY | FNOINVERT, VK_BROWSER_SEARCH, 0 },
  { FVIRTKEY | FNOINVERT, VK_BROWSER_STOP, 0 }
};

CHtmlPanelUIHandlerBase::CHtmlPanelUIHandlerBase()
{
  m_blockedAcceleratorsTable = CreateAcceleratorTable(blockedAccelerators, _countof(blockedAccelerators));
}

CHtmlPanelUIHandlerBase::~CHtmlPanelUIHandlerBase()
{
  if (m_blockedAcceleratorsTable)
  {
    DestroyAcceleratorTable(m_blockedAcceleratorsTable);
  }
}
