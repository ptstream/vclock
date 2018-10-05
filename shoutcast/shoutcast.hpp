#ifndef SHOUTCAST_HPP
#define SHOUTCAST_HPP

#include "shoutcast_global.hpp"
#include "../vtools/webradiosinterface.hpp"
#include "../vtools/nam.hpp"

/*!
 * \brief The CShoutcast class manages the Shoucast API.
 * This object needs to:
 * \li Get the list of genres avalaible on the Shoutcast servers.
 * \li Get the list of stations avalaible on the Shoutcast servers.
 * \li Get the URL of the station.
 */
class SHOUTCASTSHARED_EXPORT CShoutcast : public QObject, public CWebRadiosInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID WEBRADIOSINTERFACE_IID FILE "shoutcast.json")
  Q_INTERFACES (CWebRadiosInterface)
public:
  /*! Constructor. */
  CShoutcast ();

  /*! Desructor. */
  virtual ~CShoutcast ();

  /*! Returns a set of genres defined by Shoutcast servers.
   * \param nam : A global QNetworkAccessManager. If it is null, a new is created.
   */
  virtual QVector<CGenre> const & genres (QNetworkAccessManager* nam = nullptr);

  /*! Returns a set of stations form a genre identifier defined by Shoutcast servers.
   * \param genreId : A genre identifier returned by the above function.
   * \param nam : A global QNetworkAccessManager. If it is null, a new is created.
   */
  virtual QVector<CStation> const & stations (QVariant const & genreId, QNetworkAccessManager* nam = nullptr);

  /*! Returns a set of stations form a text (generally a part of a station name) defined by Shoutcast servers.
   * \param text : A part of a station name.
   * \param nam : A global QNetworkAccessManager. If it is null, a new is created.
   */
  virtual QVector<CStation> const & stations (QString const & text, QNetworkAccessManager* nam = nullptr);

  /*! Returns the URL of a station.
   * \param stationId : The station identifier returned by the two above functions.
   * \param nam : A global QNetworkAccessManager. If it is null, a new is created.
   */
  QUrl tunein (QVariant const & stationId, QNetworkAccessManager* nam = nullptr);

  /*! Returns the URL of a provider advertising. */
  virtual QUrl advertising () const;

  /*! Returns the URL of a provider logo. */
  virtual QPixmap logo () const;

private :
  QString m_base; //!< The base of the playlist URL for a station.
  QVariant m_currentGenreId; //!< The current genre identifier for a list of stations.
  CNam m_nam; //!< The network access manager.
};

#endif // SHOUTCAST_HPP
