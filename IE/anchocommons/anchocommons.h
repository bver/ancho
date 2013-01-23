#pragma once

#include <string>

extern const wchar_t * s_AnchoMainAPIModuleID;
extern const wchar_t * s_AnchoExtensionsRegistryKey;
extern const wchar_t * s_AnchoExtensionsRegistryEntryGUID;
extern const wchar_t * s_AnchoExtensionsRegistryEntryFlags;
extern const wchar_t * s_AnchoExtensionsRegistryEntryPath;
extern const wchar_t * s_AnchoProtocolHandlerScheme;
extern const wchar_t * s_AnchoProtocolHandlerPrefix;
extern const wchar_t * s_AnchoGlobalAPIObjectName;
extern const wchar_t * s_AnchoServiceAPIName;
extern const wchar_t * s_AnchoBackgroundAPIObjectName;
extern const wchar_t * s_AnchoBackgroundPageAPIName;
extern const wchar_t * s_AnchoBackgroundConsoleObjectName;
extern const wchar_t * s_AnchoFnGetContentAPI;
extern const wchar_t * s_AnchoFnReleaseContentAPI;
extern const wchar_t * s_AnchoTabIDPropertyName;

bool isExtensionPage(const std::wstring &aUrl);
std::wstring getDomainName(const std::wstring &aUrl);
