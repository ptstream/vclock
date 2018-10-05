#include "api_ipify_org.hpp"
#include <QJsonDocument>

CIp_ipify_org::CIp_ipify_org ()
{
}

QHostAddress CIp_ipify_org::data (QNetworkAccessManager* nam)
{
  QHostAddress addr;
  QByteArray   data = m_nam.callData (QUrl ("https://api.ipify.org/?format=json"), nam);
  if (!data.isEmpty ())
  {
    QJsonParseError     error;
    QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
    QJsonObject const & json = doc.object ();
    if (error.error == QJsonParseError::NoError)
    {
      addr.setAddress (json["ip"].toString ());
    }
  }

  return addr;
}
