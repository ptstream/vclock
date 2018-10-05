#include "ds18b20.hpp"
#include "../vtools/vtools_globals.h"
#include <QtPlugin>
#include <QFile>

CDs18b20::CDs18b20 ()
{
}

void CDs18b20::setId (QByteArray const & id)
{
  CTempSensorInterface::setId (id);
  m_fileName = QString ("/sys/bus/w1/devices/%1/w1_slave").arg (id.constData ());
}

float CDs18b20::value () const
{
  float value = VTOOLS_INVALID_TEMPERATURE;
  QFile file (m_fileName);
  if (file.open (QIODevice::ReadOnly))
  {
    QByteArray data = file.readAll ();
    int iValue      = 0;
    int index       = data.lastIndexOf ('=');
    if (index != -1)
    {
      ++index;
      int j = 0;
      if (data[index] == '-')
      {
        data[j++] = '-';
        ++index;
      }

      for (int i = index, size = data.size (); i < size; ++i)
      {
        int c = data[i];
        if (c >= 48 && c <= 57)
        {
          data[j++] = data[i];
        }
        else
        {
          break;
        }
      }

      data.truncate (j);
      iValue = data.toInt ();
    }

    file.close ();
    value = qRound (static_cast<float>(iValue) / 100.0f) / 10.0f;
  }

  return value;
}
