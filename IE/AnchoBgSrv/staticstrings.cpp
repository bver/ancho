#include "StdAfx.h"

// module ID for the main API script
LPCWSTR s_AnchoMainAPIModuleID =
    L"api.js";

// the registry key where to find extensions
// If you change this you also have to change the according entry in
// AnchoAddonService.rgs
LPCWSTR s_AnchoExtensionsRegistryKey =
    L"Software\\Salsita\\AnchoAddonService";

// registry entry "guid"
LPCWSTR s_AnchoExtensionsRegistryEntryGUID =
    L"guid";

// registry entry "Path"
LPCWSTR s_AnchoExtensionsRegistryEntryPath =
    L"Path";

// scheme name for protocol handler
LPCWSTR s_AnchoProtocolHandlerScheme =
    L"chrome-extension";

// JS object name of the API in background scripts
LPCWSTR s_AnchoGlobalAPIObjectName =
    L"addonAPI";

// JS object name of the background API in main API script
LPCWSTR s_AnchoBackgroundAPIObjectName =
    L"anchoBackgroundAPI";

// JS object name of the API in background windows.
// The name is the same both in the background window and
// in the main module exports. Means: exports.chrome in api.js
// becomes chrome in background.html
LPCWSTR s_AnchoBackgroundPageAPIName =
    L"chrome";

// name of the exported function for getting a content API
LPCWSTR s_AnchoFnGetContentAPI =
    L"getContentAPI";


// name of the exported function for releasing a content API
LPCWSTR s_AnchoFnReleaseContentAPI =
    L"releaseContentAPI";

