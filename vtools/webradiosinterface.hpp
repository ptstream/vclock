#ifndef WEBRADIOSINTERFACE_HPP
#define WEBRADIOSINTERFACE_HPP

#include "version.hpp"
#include "genre.hpp"
#include "station.hpp"
#include <QtPlugin>
#include <QVector>
#include <QPixmap>

class QNetworkAccessManager;

/*! \Brief The webradios interface plugin. */
class VTOOLSSHARED_EXPORT CWebRadiosInterface : public CVersion
{
public :
  /*! Constructor. */
  CWebRadiosInterface ();

  /*! Destructor. */
  virtual ~CWebRadiosInterface ();

  /*! Returns the list of genres.
   * \param nam: A global QNetworkAccessManager.
   */
  virtual QVector<CGenre> const & genres (QNetworkAccessManager* nam = nullptr) = 0;

  /*! Returns the list of stations from a genre identifier.
   * \param nam: A global QNetworkAccessManager.
   */
  virtual QVector<CStation> const & stations (QVariant const & genreId, QNetworkAccessManager* nam = nullptr) = 0;

  /*! Returns the list of stations from a text.
   * \param nam: A global QNetworkAccessManager.
   */
  virtual QVector<CStation> const & stations (QString const & text, QNetworkAccessManager* nam = nullptr) = 0;

  /*! Returns the tuenin url.
   * \param stationId: The station identifier.
   * \param nam: A global QNetworkAccessManager.
   */
  virtual QUrl tunein (QVariant const & stationId, QNetworkAccessManager* nam = nullptr) = 0;

  /*! Returns the advertising url of the provider. */
  virtual QUrl advertising () const = 0;

  /*! Returns the logo of the provider. */
  virtual QPixmap logo () const = 0;

  /*! Returns the genres defined by the provider. */
  QVector<CGenre> const & genres () const;

  /*! Returns the searched last stations. */
  QVector<CStation> const & stations () const;

  /*! Returns the limit of mp3 stations. */
  bool isMp3Only () const;

  /*! Keeps only the mp3 stations. */
  void setMp3Only (int mp3);

  /*! Replaces [>, <, ', ", &, &] by  [&gt, &lt, &apos;, &quot;, &amp;amp;, &amp; */
  static QString ampersandHandler (QString const & data);

  /*! Returns the url of a m3u format. */
  static QString decodeM3u (QByteArray m3u);

protected :
  bool m_mp3 = true; //!< Limit the stations with mp3 format.
  QVector<CGenre> m_genres; //!< Array of genres.
  QVector<CStation> m_stations; //!< Last array of genres.
};

#define WEBRADIOSINTERFACE_IID "vclock.nixie.CWebRadiosInterface"
Q_DECLARE_INTERFACE(CWebRadiosInterface, WEBRADIOSINTERFACE_IID)

#endif // WEBRADIOSINTERFACE_HPP
