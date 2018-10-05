#include "webradiosinterface.hpp"

CWebRadiosInterface::CWebRadiosInterface ()
{
}

CWebRadiosInterface::~CWebRadiosInterface ()
{
}

QString CWebRadiosInterface::ampersandHandler (QString const & data)
{
  QChar const      ampersand        = QLatin1Char ('&');
  QString          ampersandCodes[] = { "&gt;", "&lt;", "&apos;", "&quot;", "&amp;amp;", "&amp;" };
  QChar            replacements[]   = { '>',    '<',     '\'',    '"', '&', '&' };
  QString          dataCoded;
  int              length = data.length ();
  int              cAmpersandCodes = sizeof (ampersandCodes) / sizeof (QString);

  dataCoded.reserve (length + 50);
  for (int i = 0; i < length; ++i)
  {
    QChar c = data.at (i);
    if (c == ampersand)
    {
      int iAmpersandCode = 0;
      for (;iAmpersandCode < cAmpersandCodes; ++iAmpersandCode)
      {
        if (data.indexOf (ampersandCodes[iAmpersandCode], i) == i)
        {
          break;
        }
      }

      if (iAmpersandCode < cAmpersandCodes)
      {
        dataCoded.append (replacements[iAmpersandCode]);
        i += ampersandCodes[iAmpersandCode].length () - 1;
      }
      else
      {
        dataCoded.append ("&amp;");
      }
    }
    else
    {
      dataCoded.append (c);
    }
  }

  return dataCoded;
}

QString CWebRadiosInterface::decodeM3u (QByteArray m3u)
{
  QString url;
  m3u.replace ('\r', QString ());
  QList<QByteArray> lines = m3u.split ('\n');
  for (QByteArray const & line : lines)
  {
    if (line.startsWith ("http"))
    {
      url = QString::fromUtf8 (line);
      break;
    }
  }

  return url;
}


QVector<CGenre> const & CWebRadiosInterface::genres () const
{
  return m_genres;
}

QVector<CStation> const & CWebRadiosInterface::stations () const
{
  return m_stations;
}

bool CWebRadiosInterface::isMp3Only () const
{
  return m_mp3;
}

void CWebRadiosInterface::setMp3Only (int mp3)
{
  m_mp3 = mp3;
}
