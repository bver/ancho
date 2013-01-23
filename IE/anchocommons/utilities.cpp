#include "anchocommons.h"

//----------------------------------------------------------------------------
//
bool isExtensionPage(const std::wstring &aUrl)
{
  return std::wstring::npos != aUrl.find(s_AnchoProtocolHandlerPrefix);
}
//----------------------------------------------------------------------------
//
std::wstring getDomainName(const std::wstring &aUrl)
{
  static const std::wstring protocolDelimiter = L"://";
  size_t prefix = aUrl.find(protocolDelimiter);
  if (prefix == std::wstring::npos) {
    throw std::runtime_error("Wrong URL format");
  }
  size_t domainStart = prefix + protocolDelimiter.size();

  size_t domainEnd = aUrl.find_first_of(L'/', domainStart);

  return aUrl.substr(domainStart, domainEnd-domainStart);
}

