#ifndef WEBRADIO_HPP
#define WEBRADIO_HPP

#include "../vtools/station.hpp"
#ifdef QTMULTIMEDIA_DEFINED
#include <QMediaPlayer>
#else
#include "mediaplayer.hpp"
#include <QVector>
#endif

class CWebRadiosInterface;

/*! \brief This class manages a list of Webradios.
 * The current webradio can be palyer on the mediaplayer.
 * This object is similar at a playlist.
 */
class VTOOLSSHARED_EXPORT CWebRadio : public QMediaPlayer
{
public:
  /*! Constructor. */
  CWebRadio (QObject* parent = nullptr);

  /*! Returns the lists of station names. */
  QStringList names () const;

  /*! Returns the current played station name. */
  QString currentName (int len = -1) const;

  /*! Returns the current played station index. */
  int currentIndex () const { return m_currentIndex; }

  /*! Sets the current played station index. */
  void setCurrentIndex (int index);

  /*! Returns the station index from an url. */
  int index (QUrl const & url);

  /*! Returns the current station logo url. */
  QUrl currentLogo () const;

  /*! Adds a new station. */
  void add (CStation const & station);

  /*! Removes a stations list. */
  void del (QList<int> indices);

  /*! Returns true if the player is currently in playing mode. */
  bool isPlaying () const { return state () == QMediaPlayer::PlayingState; }

#ifndef QTMULTIMEDIA_DEFINED
  /*! Sets the external player program.
   * For Rasbian, the mediaplayer is handled by an external program.
   * For example mpg123 or VLC.
   */
  void setPlayerProgram (QString const & player);

  /*! Returns the external player program.
   * For Rasbian, the mediaplayer is handled by an external program.
   * For example mpg123 or vlc.
   */
  QList<QByteArray> const & playerProgram () const { return m_playerComs; }

  /*! Sets the external mixer program.
   * For Rasbian, the mixer is handled by an external program.
   * For example amixer.
   */
  void setMixerProgram (QString const & mixer);

  /*! Returns the external mixer program.
   * For Rasbian, the mixer is handled by an external program.
   * For example amixer.
   */
  QString const & mixerProgram () const { return m_mixerProgram; }
#endif

  /*! Returns true if the webradios are limited at mp3 format. */
  bool isMp3Only () const { return m_mp3; }

  /*! Limits the webradios with mp3 format. */
  void setMp3Only (bool mp3) { m_mp3 = mp3; }

  /*! Returns the lists of webstations. */
  QVector<CStation> const & stations () const { return m_stations; }

  /*! Returns the lists of webstations. */
  QVector<CStation>& stations () { return m_stations; }

  /*! Sets a lists of webstations. */
  void setStations (QVector<CStation> const & stations) { m_stations = stations; }

private :
  int m_currentIndex = -1; //!< The current station index.
  bool m_mp3 = false; //!< Keep mp3 station only.
  QVector<CStation> m_stations; //!< The list of stations.
};

#endif // WEBRADIO_HPP
