#ifndef MEDIAPLAYER_HPP
#define MEDIAPLAYER_HPP

#ifndef QTMULTIMEDIA_DEFINED
#include "../vtools/vtools_globals.h"
#include <QObject>
#include <QUrl>

class QProcess;

class VTOOLSSHARED_EXPORT QMediaPlayer : public QObject
{
  Q_OBJECT
public :
  enum State { StoppedState, PlayingState, PausedState };
  enum Commands { PlayerName, PlayerPlay, PlayerQuit, PlayerEndOfTrack, PLayerInit };

  QMediaPlayer (QObject* parent = nullptr);
  virtual ~QMediaPlayer ();

  void setVolume (int volume);
  int volume () const { return m_volume; }
  bool isPlaying () const;
  void play ();
  void stop ();
  bool isMuted () const;
  void setMuted (bool mute);
  State state () const;
  QUrl media () const { return m_url; }
  void setMedia (QUrl const & url);
  QProcess* player () { return m_player; }
  void setPlayerProgram (QString const & player);
  void setMixerProgram (QString const & mixer) { m_mixerProgram = mixer; }
  void setPlayerComs (QList<QByteArray> const & coms) { m_playerComs = coms; }
  QList<QByteArray> const & playerComs () { return m_playerComs; }
  void playTrack ();

protected slots :
  void readyReadStandardOutput ();
  void readyReadStandardError ();

signals :
  void trackEnded ();

protected :
  State m_state = StoppedState;
  QUrl m_url;
  int m_volume = 20;
  QProcess* m_player = nullptr;
  QList<QByteArray> m_playerComs;
  QString m_mixerProgram;
  int m_timeout = 5000;
};

#endif //QTMULTIMEDIA_DEFINED
#endif // MEDIAPLAYER_HPP
