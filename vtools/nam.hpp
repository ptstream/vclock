#ifndef NAM_HPP
#define NAM_HPP

#include "vtools_globals.h"
#include <QEventLoop>
#include <QNetworkReply>

class QUrl;

/*! \brief The network access manager.
 * This object is allowed to call a server throw an url and return data from the server.
 */
class VTOOLSSHARED_EXPORT CNam : public QEventLoop
{
  Q_OBJECT
public:
  /*! Constructor. */
  CNam (QObject* parent = nullptr);

  /*! Destructor. */
  virtual ~CNam ();

  /*! Returns the data returned by the server at url.
   * \param url: URL to call.
   * \param name: The global QNetworkAccessManager.
   */
  QByteArray callData (QUrl const & url, QNetworkAccessManager* nam = nullptr);

protected slots :
  /*! The error generated by the server. */
  void error (QNetworkReply::NetworkError err);

  /*! The server has finished to sent data. */
  void finished ();

protected :
  /*! Timer event in case of no possibility to contact the server. */
  virtual void timerEvent (QTimerEvent*);

protected :
  int m_timeout = 30000; //!< Maximum timout for the server response.
};

#endif // NAM_HPP
