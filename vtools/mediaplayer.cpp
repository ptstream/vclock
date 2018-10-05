#include "webradio.hpp"

#ifndef QTMULTIMEDIA_DEFINED
#include <QDebug>
#include <QProcess>

static bool decodeMute (QString const & program, QByteArray const & read)
{
  bool mute = false;
  if (program == "amixer")
  {
    int index = read.lastIndexOf ('[');
    if (index != -1)
    {
      QString s = read.mid (index);
      index = read.toLower ().indexOf ("off", index);
      mute  = index != -1;
    }
  }

  return mute;
}

QMediaPlayer::QMediaPlayer (QObject* parent) : QObject (parent)
{
  m_player = new QProcess (this);
  connect (m_player, &QProcess::readyReadStandardOutput, this, &QMediaPlayer::readyReadStandardOutput);
  connect (m_player, &QProcess::readyReadStandardError, this, &QMediaPlayer::readyReadStandardError);
}

QMediaPlayer::~QMediaPlayer ()
{
  stop ();
}

void QMediaPlayer::setPlayerProgram (QString const & player)
{
  QStringList coms  = player.split ('|');
  int         count = coms.size ();
  if (count >= PlayerQuit)
  {
    m_playerComs.reserve (count);
    for (int i = 0; i < count; ++i)
    {
      QByteArray bytes = coms[i].toLatin1 ();
      if (i != PlayerPlay && i != PlayerEndOfTrack)
      {
        bytes.push_back ('\n');
      }

      m_playerComs.push_back (bytes);
    }
  }
}

void QMediaPlayer::setVolume (int volume)
{
  m_volume = volume;
  QProcess amixer;
  QStringList args;
  args << "cset" << "numid=1" << QString ("%1\%").arg (volume);
  qDebug () << args;
  amixer.startDetached (m_mixerProgram, args);
}

bool QMediaPlayer::isPlaying () const
{
  return m_state == PlayingState;
}

void QMediaPlayer::playTrack ()
{
  if (!m_playerComs.isEmpty ())
  {
    QByteArray command = m_url.toString ().toLatin1 ();
    if (command.startsWith ("file://"))
    {
      command = command.mid (7);
    }

    command.prepend (m_playerComs[PlayerPlay]);
    command += '\n';
    m_player->write (command);
  }
}

void QMediaPlayer::play ()
{
  if (!m_playerComs.isEmpty () && m_state == StoppedState)
  {
    m_player->start (m_playerComs[PlayerName]);
    m_player->waitForStarted (m_timeout);
    for (int i = PLayerInit, count = m_playerComs.size (); i < count; ++i)
    {
      m_player->write (m_playerComs[i]);
    }

    playTrack ();
    m_state = PlayingState;
  }
}

void QMediaPlayer::stop ()
{
  if (!m_playerComs.isEmpty () && m_state != StoppedState)
  {
    m_player->write (m_playerComs[PlayerQuit]);
    m_player->waitForFinished (m_timeout);
    m_state = StoppedState;
  }
}

bool QMediaPlayer::isMuted () const
{
  QProcess driver;
  driver.start (m_mixerProgram);
  driver.waitForReadyRead ();
  QByteArray ret = driver.readAllStandardOutput ();
  bool mute = decodeMute (m_mixerProgram, ret);
  driver.waitForFinished ();
  return mute;
}

void QMediaPlayer::setMuted (bool mute)
{
  QProcess amixer;
  QStringList args;
  args << "set" << "PCM" << (mute ? "mute" : "unmute");
  amixer.startDetached (m_mixerProgram, args);
}

QMediaPlayer::State QMediaPlayer::state () const
{
  return m_state;
}

void QMediaPlayer::setMedia (QUrl const & url)
{
  stop ();
  m_url = url;
}

void QMediaPlayer::readyReadStandardOutput ()
{
  QByteArray outputs = m_player->readAllStandardOutput ();
  if (!m_playerComs[PlayerEndOfTrack].isEmpty () && outputs.contains (m_playerComs[PlayerEndOfTrack]))
  {
    emit trackEnded ();
  }
}

void QMediaPlayer::readyReadStandardError ()
{
  QByteArray errors = m_player->readAllStandardError ();
  qDebug () << errors;
}

#endif // QTMULTIMEDIA_DEFINED

