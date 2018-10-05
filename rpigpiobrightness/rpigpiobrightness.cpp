#include "rpigpiobrightness.hpp"
#include <QFile>
#include <QDir>

CRpiGpioBrightness::CRpiGpioBrightness ()
{
}

CRpiGpioBrightness::~CRpiGpioBrightness ()
{
  QFile file (m_dir + "unexport");
  if (file.open (QIODevice::WriteOnly))
  {
    file.write (m_id);
  }
}

bool CRpiGpioBrightness::connected () const
{
  bool c = m_connected;
  if (c)
  {
    QDir dir (m_dir + QString ("gpio%1").arg (m_id.constData ()));
    c = dir.exists ();
  }

  return c;
}

void CRpiGpioBrightness::setId (QByteArray const & id)
{
  bool ok;
  int  pins[] = { 17, 18, 27, 22, 23, 24, 25, 4, 28, 29, 30, 31 };
  int  pin    = id.toInt (&ok);
  int  count  = static_cast<int>(sizeof (pins) / sizeof (int));
  if (ok && pin >= 0 && pin < count)
  {
    CBrightnessSensorInterface::setId (QByteArray::number (pins[pin]));
    QFile file (m_dir + "export");
    if (file.open (QIODevice::WriteOnly))
    {
      qint64 written = file.write (this->id ());
      m_connected    = written == id.size ();
    }
  }
}

float CRpiGpioBrightness::value () const
{
  float   value    = -1.0f;
  QString fileName = m_dir + QString ("gpio%1/value").arg (m_id.constData ());
  QFile file (fileName);
  if (file.open (QIODevice::ReadOnly))
  {
    QByteArray content = file.readAll ();
    if (content.size () >= 1)
    {
      value = content[0] == '0' ? 1.0f : 0.0f;
    }
  }

  return value;
}
