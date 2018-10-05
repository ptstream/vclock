#include "nam.hpp"

CNam::CNam (QObject* parent) : QEventLoop (parent)
{
}

CNam::~CNam ()
{
}

QByteArray CNam::callData (QUrl const & url, QNetworkAccessManager* nam)
{
  if (nam == nullptr)
  {
    nam = new QNetworkAccessManager (this);
  }

  QByteArray      data;
  QNetworkRequest nreq (url);
  nreq.setSslConfiguration (QSslConfiguration::defaultConfiguration ());
  QNetworkReply* reply = nam->get (nreq);
  if (reply != nullptr)
  {
    connect (reply, SIGNAL (error (QNetworkReply::NetworkError)), this, SLOT (error (QNetworkReply::NetworkError)));
    connect (reply, SIGNAL (finished ()), this, SLOT (finished ()));
    int idTimer = startTimer (m_timeout);
    exec (QEventLoop::ExcludeUserInputEvents);
    killTimer (idTimer);
    data = reply->readAll ();
    reply->deleteLater ();
  }

  return data;
}

void CNam::error (QNetworkReply::NetworkError err)
{
  QNetworkReply* replySender = static_cast<QNetworkReply*>(sender ());
  QString        errorString = replySender->errorString ();
  qDebug () << "CNam::error: " << err << " (" << errorString << ")";
  quit ();
}

void CNam::finished ()
{
  quit ();
}

void CNam::timerEvent (QTimerEvent*)
{
  qDebug () << "CNam::timerEvent";
  quit ();
}


