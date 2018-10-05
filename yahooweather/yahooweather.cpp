#include "yahooweather.hpp"
#include "../vtools/nam.hpp"
#include "../vtools/vtools_globals.h"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

CYahooWeather::CYahooWeather ()
{
}

CYahooWeather::~CYahooWeather ()
{
}

int CYahooWeather::count () const
{
  return m_forecasts.size ();
}

QVector<CForecast> const & CYahooWeather::data (QNetworkAccessManager* nam)
{
  if (!m_town.isEmpty () && !m_country.isEmpty ())
  {
    QString urlTemplate = "https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22%9998%2C%20%9999%22)&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";
    urlTemplate.replace ("%9998", m_town); // Town
    urlTemplate.replace ("%9999", m_country); // Country
    QByteArray data = m_nam.callData (QUrl (urlTemplate), nam);
    decode (data);
  }

  return m_forecasts;
}

QPixmap CYahooWeather::pixmap (int code)
{
  if (code > 47)
  {
    code = 3200;
  }

  return QPixmap (QString (":/yahooicons/%1.png").arg (code));
}

QUrl CYahooWeather::advertising () const
{
  return QUrl ("https://www.yahoo.com/?ilc=401");
}

QPixmap CYahooWeather::logo () const
{
  return QPixmap (QString (":/yahhoicons/logo.png"));
}

void CYahooWeather::decode (QByteArray const & data)
{
  auto temperature = [] (QJsonValue const & value) -> float
  {
    bool  ok;
    float v = value.toString ().toFloat (&ok);
    return !ok ? VTOOLS_INVALID_TEMPERATURE : ::toCelcius (v);
  };

  auto code = [] (QJsonValue const & value) -> int
  {
    bool ok;
    int  v = value.toString ().toInt (&ok);
    return !ok ? 3200 : v;
  };

  m_forecasts.clear ();
  QJsonParseError     error;
  QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
  QJsonObject const & json = doc.object ();
  if (error.error == QJsonParseError::NoError)
  {
    QJsonObject query     = json["query"].toObject ();
    QJsonObject results   = query["results"].toObject ();
    QJsonObject channel   = results["channel"].toObject ();
    QJsonObject item      = channel["item"].toObject ();
    QJsonObject condition = item["condition"].toObject ();
    QJsonArray  forecast  = item["forecast"].toArray ();
    int         count     = forecast.size ();
    m_forecasts.reserve (count);

    CForecast f;
    f.setTemp (temperature (condition["temp"]));
    f.setText (condition["text"].toString ());
    f.setCode (code (condition["code"]));
    QJsonObject value = forecast.at (0).toObject ();
    f.setTempMin (temperature (value["low"]));
    f.setTempMax (temperature (value["high"]));
    m_forecasts.push_back (f);
    for (int i = 1, count = forecast.size (); i < count; ++i)
    {
      QJsonObject value = forecast.at (i).toObject ();
      f.setCode (code (value["code"]));
      f.setTempMin (temperature (value["low"]));
      f.setTempMax (temperature (value["high"]));
      f.setText (value["text"].toString ());
      f.setTemp (VTOOLS_INVALID_TEMPERATURE);
      m_forecasts.push_back (f);
    }
  }
}


