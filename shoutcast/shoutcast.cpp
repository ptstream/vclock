#include "shoutcast.hpp"
#include <QJsonDocument>
#include <QJsonArray>
#include <QBuffer>
#include <QXmlSimpleReader>

static char const * const shoutcastKey = "5gDMNeruqv9PVEiS";

class CXmlHandler : public QXmlDefaultHandler
{
public :
  CXmlHandler (QVector<CStation>& stations, QString& base);

  /*! Parse the XML response. */
  virtual bool parse (QByteArray data);

  /*! The reader calls this function when it has parsed a start element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool startElement (QString const &, QString const &, QString const & qName, QXmlAttributes const & atts);

private :
  QVector<CStation>& m_stations;
  QString m_base;
};

CXmlHandler::CXmlHandler (QVector<CStation>& stations, QString& base) : m_stations (stations), m_base (base)
{
}

bool CXmlHandler::parse (QByteArray data)
{
  bool             done = false;
  QBuffer          buffer (&data);
  QXmlSimpleReader reader;
  reader.setContentHandler (this);
  QXmlInputSource  source (&buffer);
  done = reader.parse (source);
  return done;
}


bool CXmlHandler::startElement (QString const &, QString const &, QString const & qName, QXmlAttributes const & atts)
{
  if (qName == "tunein")
  {
    m_base = atts.value ("base-m3u");
  }
  else if (qName == "station")
  {
    CStation station;
    for (int iAtt = 0, cAtts = atts.count (); iAtt < cAtts; ++iAtt)
    {
      QString const & name = atts.qName (iAtt);
      if (name == "logo")
      {
        station.setLogo (QUrl (atts.value (iAtt)));
      }
      else if (name == "name")
      {
        station.setName (atts.value (iAtt));
      }
      else if (name == "id")
      {
        station.setId (atts.value (iAtt).toUtf8 ());
      }
    }

    m_stations.push_back (station);
  }

  return true;
}

CShoutcast::CShoutcast ()
{
}

CShoutcast::~CShoutcast ()
{
}

QVector<CGenre> const & CShoutcast::genres (QNetworkAccessManager* nam)
{
  if (m_genres.isEmpty ())
  {
    QUrl       url (QString ("http://api.shoutcast.com/genre/primary?k=%1&f=json").arg (shoutcastKey));
    QByteArray data = m_nam.callData (url, nam);
    if (!data.isEmpty ())
    {
      QJsonParseError     error;
      QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
      QJsonObject const & json = doc.object ();
      if (error.error == QJsonParseError::NoError)
      {
        QJsonObject const & jResponse  = json["response"].toObject ();
        QJsonObject const & jData      = jResponse["data"].toObject ();
        QJsonObject const & jGenreList = jData["genrelist"].toObject ();
        QJsonArray const &  jGenre     = jGenreList["genre"].toArray ();
        m_genres.reserve (jGenre.size ());
        for (QJsonArray::const_iterator it = jGenre.begin (), end = jGenre.end (); it != end; ++it)
        {
          QJsonObject const & g    = (*it).toObject ();
          QString             name = ampersandHandler (g["name"].toString ());
          QByteArray          id   = QByteArray::number (g["id"].toInt ());
          CGenre              genre (name, id);
          m_genres.push_back (genre);
        }
      }
    }
  }

  return m_genres;
}

QVector<CStation> const & CShoutcast::stations (QVariant const & genreId, QNetworkAccessManager* nam)
{
  if (genreId != m_currentGenreId)
  {
    m_stations.clear ();
    m_currentGenreId = genreId;
    QString url = QString ("http://api.shoutcast.com/station/advancedsearch?genre_id=%1&f=json&k=%2")
                  .arg (genreId.toInt ()).arg (shoutcastKey);
    if (m_mp3)
    {
      url += "&mt=audio/mpeg";
    }

    QByteArray data = m_nam.callData (url, nam);
    if (!data.isEmpty ())
    {
      QJsonParseError     error;
      QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
      QJsonObject const & json = doc.object ();
      if (error.error == QJsonParseError::NoError)
      {
        QJsonObject const & jResponse    = json["response"].toObject ();
        QJsonObject const & jData        = jResponse["data"].toObject ();
        QJsonObject const & jStationList = jData["stationlist"].toObject ();
        QJsonArray const &  jStation     = jStationList["station"].toArray ();
        m_stations.reserve (jStation.size ());
        for (QJsonArray::const_iterator it = jStation.begin (), end = jStation.end (); it != end; ++it)
        {
          QJsonObject const & g    = (*it).toObject ();
          QString             name = ampersandHandler (g["name"].toString ());
          if (name.isEmpty ())
          {
            name = tr ("Unknown title");
          }

          QString logo = g["logo"].toString ();
          int     id   = g["id"].toInt ();
          CStation station (name, id, logo);
          m_stations.push_back (station);
        }

        std::sort (m_stations.begin (), m_stations.end (),
                   [] (CStation const & s1, CStation const & s2) -> bool { return s1.name () < s2.name (); });

        QJsonObject const & jTunein  = jStationList["tunein"].toObject ();
        m_base                       = jTunein["base-m3u"].toString ();
      }
    }
  }

  return m_stations;
}

QVector<CStation> const & CShoutcast::stations (QString const & text, QNetworkAccessManager* nam)
{
  QString url = QString ("http://api.shoutcast.com/legacy/stationsearch?search=%1&k=%2")
                .arg (text.toUtf8 ().constData ()).arg(shoutcastKey);
  if (m_mp3)
  {
    url += "&mt=audio/mpeg";
  }

  m_stations.clear ();
  QByteArray data = m_nam.callData (url, nam);
  if (!data.isEmpty ())
  {
    CXmlHandler h (m_stations, m_base);
    h.parse (data);
  }

  return m_stations;
}

QUrl CShoutcast::tunein (QVariant const & id, QNetworkAccessManager* nam)
{
  QUrl       url (QString ("http://yp.shoutcast.com%1?id=%2").arg (m_base).arg (id.toInt ()));
  QByteArray data = m_nam.callData (url, nam);
  url             = decodeM3u (data);
  return url;
}

QUrl CShoutcast::advertising () const
{
  return QUrl ("http://www.shoutcast.com/");
}

QPixmap CShoutcast::logo () const
{
  return QPixmap (QString (":/shoutcasticons/logo.png"));
}
