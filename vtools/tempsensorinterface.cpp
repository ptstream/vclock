#include "tempsensorinterface.hpp"

CTempSensorInterface::CTempSensorInterface ()
{
}

CTempSensorInterface::~CTempSensorInterface ()
{
}

void CTempSensorInterface::setId (QByteArray const & id)
{
  m_id = id;
}

QByteArray const & CTempSensorInterface::id () const
{
  return m_id;
}
