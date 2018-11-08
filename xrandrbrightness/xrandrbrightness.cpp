#include "xrandrbrightness.hpp"
#include <QProcess>

CXrandrBrightness::CXrandrBrightness ()
{
  QProcess process;
  process.setProcessChannelMode(QProcess::MergedChannels);
  process.start ("xrandr -q");
  process.waitForStarted ();
  process.waitForFinished ();
  QByteArray        ret   = process.readAll ();
  QList<QByteArray> lines = ret.split ('\n');
  for (QByteArray const & line : lines)
  {
    if (line.contains ("primary"))
    {
      int pos = line.indexOf (' ');
      if (pos != -1)
      {
        m_displayName = line.left (pos);
        break;
      }
    }
  }
}

float CXrandrBrightness::value () const
{
  float value = 1.0f;
  QProcess process;
  process.start ("xrandr -q --verbose");
  process.waitForStarted ();
  process.waitForFinished ();
  QByteArray        ret   = process.readAll ();
  QList<QByteArray> lines = ret.split ('\n');
  for (QByteArray const & line : lines)
  {
    if (line.contains ("Brightness"))
    {
      QList<QByteArray> br = line.split (' ');
      if (br.size () == 2)
      {
        bool ok;
        value = br.last ().toFloat (&ok);
        if (!ok)
        {
          value = 1.0;
        }
        else
        {
          value = static_cast<int>(value * 1000.0f) / 1000.0f;
        }
        break;
      }
    }
  }

  return value;
}

bool CXrandrBrightness::setValue (float value)
{
  QString com = QString ("xrandr --output %1 --brightness %2").arg (m_displayName).arg (value);
  QProcess process;
  process.start (com);
  process.waitForStarted ();
  process.waitForFinished ();
  return true;
}

bool CXrandrBrightness::connected () const
{
  return !m_displayName.isEmpty ();
}


