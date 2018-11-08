#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "webradiosmanager.hpp"
#include "../vtools/webradio.hpp"
#include "../vtools/helper.hpp"
#include <QMenu>

void CMainWindow::on_m_audioPlay_clicked ()
{
  if (m_webRadios.currentIndex () != -1)
  {
    QString iconName;
    if (m_webRadios.isPlaying ())
    {
      m_webRadios.stop ();
      iconName = "stopped";
    }
    else
    {
      m_webRadios.play ();
      iconName = "playing";
    }

    ui->m_audioPlay->setIcon (QIcon (":/icons/" + iconName + ".png"));
    updateSleeping ();
  }
}

void CMainWindow::on_m_mute_clicked ()
{
  bool mute = !m_webRadios.isMuted ();
  m_webRadios.setMuted (mute);
  QString iconName = mute ? "mute" : "sound";
  ui->m_mute->setIcon (QIcon (":/icons/" + iconName + ".png"));
}

void CMainWindow::sleeping ()
{
  auto action        = static_cast<QAction*>(sender ());
  m_sleepingInterval = sleepingInterval (action); // Minutes
  if (m_sleepingInterval == 0)
  {
    m_sleepingTimer.stop ();
  }
  else
  {
    m_sleepingTimer.start (m_sleepingInterval * 60000);
  }

  updateSleeping ();
}

void CMainWindow::on_m_volume_valueChanged (int value)
{
  m_webRadios.setVolume (value);
}

void CMainWindow::showHidePlayer ()
{
  bool hide = !ui->m_audioPlay->isHidden ();
  ui->m_audioPlay->setHidden (hide);
  ui->m_mute->setHidden (hide);
  ui->m_volume->setHidden (hide);
  ui->m_station->setHidden (hide);
  ui->m_logo->setHidden (hide);
  ui->m_webRadiosLogo->setHidden (hide);
  ui->m_sleeping->setHidden (hide);
  if (!hide)
  {
    QString name = m_webRadios.currentName (m_stationNameLength);
    if (name.isEmpty ())
    {
      name = tr ("No webradio");
    }

    ui->m_station->setText (name);
    QString iconName = m_webRadios.isMuted () ? "mute" : "sound";
    ui->m_mute->setIcon (QIcon (":/icons/" + iconName + ".png"));
    if (ui->m_logo->pixmap () == nullptr)
    {
      QUrl    logoUrl = m_webRadios.currentLogo ();
      QPixmap pixmap  = ::logo (logoUrl, m_nam);
      ui->m_logo->setPixmap (pixmap);
    }

    ui->m_volume->blockSignals (true);
    ui->m_volume->setValue (m_webRadios.volume ());
    ui->m_volume->blockSignals (false);
  }

  update ();
}

void CMainWindow::sleepTimeout ()
{
  if (m_webRadios.isPlaying ())
  {
    on_m_audioPlay_clicked ();
  }
}
