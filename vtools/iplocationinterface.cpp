#include "iplocationinterface.hpp"

CIpLocationInterface::CIpLocationInterface ()
{
}

CIpLocationInterface::~CIpLocationInterface ()
{
}

void CIpLocationInterface::setExternalAddress (QHostAddress const & addr)
{
  m_addr = addr;
}
