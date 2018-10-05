#include "brightnesssensorinterface.hpp"

CBrightnessSensorInterface::CBrightnessSensorInterface ()
{
}

CBrightnessSensorInterface::~CBrightnessSensorInterface ()
{
}

void CBrightnessSensorInterface::setId (QByteArray const & id)
{
  m_id = id;
}

QByteArray const & CBrightnessSensorInterface::id () const
{
  return m_id;
}
