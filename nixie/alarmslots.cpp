#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "alarmmanager.hpp"
#include "../vtools/alarm.hpp"
#include "../vtools/helper.hpp"

void CMainWindow::alarms ()
{
  ui->m_frameCount->clear ();
  ui->m_forecasts->setHidden (true);
  CAlarmManager alarmManager (m_alarms, this);
  alarmManager.exec ();
  launchAlarms ();
}

void CMainWindow::alarmStopped ()
{
  ring ();
  CAlarm* alarm = static_cast<CAlarm*>(sender ());
  updateAlarmButtons ();
  m_alarms.removeOne (alarm);
  delete alarm;
}

void CMainWindow::alarmRestarted ()
{
  updateAlarmButtons ();
}

void CMainWindow::alarmRepeatStarted (unsigned index)
{
  ring ();
  CAlarm*      alarm = static_cast<CAlarm*>(sender ());
  QToolButton* btn   = static_cast<QToolButton*>(alarm->widget ());
  if (btn != nullptr)
  {
    QPixmap pxm (":/icons/alarm.png");
    QColor  color = alarmIndexColor (m_primaryColor);
    pxm           = addIndex (pxm, index, color, true);
    btn->setIcon (QIcon (pxm));
  }
}

void CMainWindow::ring ()
{
  CMode* mode = ui->m_device->mode ();
  if (mode != nullptr)
  {
    QUrl url;
    if (mode->type () != CMode::Timer)
    {
      m_ringtoneVolume = m_ringtoneVolumeMin;
      url              = m_alarmRingtoneURL;
    }
    else
    {
      m_ringtoneVolume = m_ringtoneVolumeMax;
      url              = m_timerRingtoneURL;
      ui->m_swStartPause->setIcon (QIcon (QString (":/icons/play.png")));
    }

    if (!url.isEmpty () && m_ringtonePlayer->state () != QMediaPlayer::PlayingState)
    {
      m_ringtonePlayer->setVolume (m_ringtoneVolume);
      m_ringtonePlayer->setMedia (url);
      m_ringtonePlayer->play ();
      if (m_ringtoneVolume != m_ringtoneVolumeMax)
      {
        m_ringtoneVolumeTimer.start (m_ringtoneIncreaseInterval);
        m_ringtoneMS = 0;
      }

      ui->m_device->setDynamicRotationEnabled (false);
    }
  }
}

void CMainWindow::ringtoneIncreaseVolume ()
{
  QMediaPlayer::State state = m_ringtonePlayer->state ();
  if (state == QMediaPlayer::PlayingState)
  {
    m_ringtoneMS += m_ringtoneIncreaseInterval;
    if (m_ringtoneVolume < m_ringtoneVolumeMax)
    {
      m_ringtoneVolume += m_ringtoneVolumeStep;
      m_ringtonePlayer->setVolume (m_ringtoneVolume);
    }
  }

  if (m_ringtoneMS > m_ringtoneDuration)
  {
    m_ringtoneVolumeTimer.stop ();
  }
}

void CMainWindow::on_m_a1_clicked ()
{
  alarmClicked (sender ());
}

void CMainWindow::on_m_a2_clicked ()
{
  alarmClicked (sender ());
}

void CMainWindow::on_m_a3_clicked ()
{
  alarmClicked (sender ());
}

#ifdef QTMULTIMEDIA_DEFINED
void CMainWindow::ringtoneStateChanged (QMediaPlayer::State newState)
{
  if (newState == QMediaPlayer::StoppedState)
  {
    ui->m_device->setDynamicRotationEnabled (true);
    if (m_ringtoneVolumeTimer.isActive ())
    {
      m_ringtonePlayer->play ();
      ui->m_device->setDynamicRotationEnabled (false);
    }
  }
}
#else
void CMainWindow::ringtoneFinished ()
{
  ui->m_device->setDynamicRotationEnabled (true);
  if (m_ringtoneVolumeTimer.isActive ())
  {
    m_ringtonePlayer->playTrack ();
  }
}
#endif
