#include "rpiblbrightness.hpp"
#include <QFileInfo>

CRpiBlBrightness::CRpiBlBrightness ()
{
}

QByteArray CRpiBlBrightness::read () const
{
  QByteArray data;
  QFile      file (m_fileName);
  if (file.open (QIODevice::ReadOnly))
  {
    data = file.readAll ();
    file.close ();
  }

  return data;
}

bool CRpiBlBrightness::write (int value)
{
  bool  done = false;
  QFile file (m_fileName);
  if (file.open (QIODevice::WriteOnly))
  {
    QByteArray data = QByteArray::number(value) + '\n';
    done            = file.write (data) == data.size ();
    file.close ();
  }

  return done;
}

float CRpiBlBrightness::value () const
{
  int        value = -1;
  QByteArray data  = read ();
  if (!data.isEmpty ())
  {
    data.truncate (data.size () - 1); // Remove '\n'
    bool ok;
    value = data.toInt (&ok);
    if (!ok && value < m_min && value > m_max)
    {
      value = -1;
    }
  }

  return (static_cast<float>(value) - m_min) / (m_max - m_min);
}

bool CRpiBlBrightness::setValue (float value)
{
  bool done = false;
  if (value >= 0.0f && value <= 1.0f)
  {
    int iValue = qRound ((m_max - m_min) * value + m_min);
    done  = write (iValue);
  }

  return done;
}

bool CRpiBlBrightness::connected () const
{
  return QFileInfo (m_fileName).exists ();
}


