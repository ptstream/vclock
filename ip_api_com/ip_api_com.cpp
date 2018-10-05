#include "ip_api_com.hpp"
#include <QJsonDocument>
#include <QJsonObject>

CIp_api_com::CIp_api_com ()
{
}

QPair<QString, QString> CIp_api_com::data (QNetworkAccessManager* nam)
{
  QPair<QString, QString> values;
  QUrl                    url ("http://ip-api.com/json/" + m_addr.toString ());
  QByteArray              data = m_nam.callData (url, nam);
  if (!data.isEmpty ())
  {
    QJsonParseError     error;
    QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
    QJsonObject const & json = doc.object ();
    if (error.error == QJsonParseError::NoError)
    {
      values.first  = json["city"].toString ();
      values.second = json["countryCode"].toString ();
    }
  }

  return values;
}
