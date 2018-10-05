#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../vmode/stopwatch.hpp"
#include "../vmode/timer.hpp"

static void updateIcons (Ui::CMainWindow* ui, CStopwatch const * stopwatch)
{
  char const * icon = stopwatch->playing () ? "pause": "play";
  ui->m_swStartPause->setIcon (QIcon (QString (":/icons/") + icon + ".png"));

  icon = stopwatch->tour () ? "tour": "reset";
  ui->m_swTourReset->setIcon (QIcon (QString (":/icons/") + icon + ".png"));
}

static void updateIcons (Ui::CMainWindow* ui, CTimer const * timer)
{
  char const * icon = timer->playing () ? "pause": "play";
  ui->m_swStartPause->setIcon (QIcon (QString (":/icons/") + icon + ".png"));
}

static QString formatText (QVector<int> const & tours)
{
  QString text;
  for (int i = 0, count = tours.size () - 1; i < count; ++i)
  {
    int diff = tours[i];
    if (i != 0)
    {
      diff -= tours[i - 1];
    }

    QString t0 (QTime (0, 0).addMSecs (diff).toString ("mm ss.zzz"));
    t0.truncate (t0.length () - 1);

    QString t1 (QTime (0, 0).addMSecs (tours[i]).toString ("mm ss.zzz"));
    t1.truncate (t1.length () - 1);
    if (i > 0)
    {
      text.prepend ('\n');
    }

    text.prepend (QString ("#%1 %2 %3").arg (i + 1).arg (t0).arg (t1));
  }

  return text;
}

void CMainWindow::on_m_swStartPause_clicked ()
{
  CMode* mode = ui->m_device->mode ();
  switch (mode->type ())
  {
    case CMode::Stopwatch :
    {
      CStopwatch* stopwatch = static_cast<CStopwatch*>(ui->m_device->mode ());
      stopwatch->startPause ();
      updateIcons (ui, stopwatch);
      int interval = mode->interval ();
      if (interval == -1)
      {
        ui->m_device->redrawTimer ().stop ();
      }
      else
      {
        ui->m_device->redrawTimer ().start ();
      }
      break;
    }

    case CMode::Timer :
    {
      if (m_ringtonePlayer->state () == QMediaPlayer::PlayingState)
      {
        m_ringtonePlayer->stop ();
      }

      CTimer* timer = static_cast<CTimer*>(ui->m_device->mode ());
      timer->swapState ();
      updateIcons (ui, timer);
      break;
    }

    default :
      break;
  }
}

void CMainWindow::on_m_swTourReset_clicked ()
{
  CMode* mode = ui->m_device->mode ();
  switch (mode->type ())
  {
    case CMode::Stopwatch :
    {
      CStopwatch* stopwatch = static_cast<CStopwatch*>(ui->m_device->mode ());
      stopwatch->tourReset ();
      updateIcons (ui, stopwatch);
      if (!stopwatch->playing ())
      {
        ui->m_device->updateTubes ();
        ui->m_frameCount->clear ();
      }

      if (stopwatch->tour ())
      {
        QVector<int> const & tours = stopwatch->tours ();
        QString              text  = formatText (tours);
        ui->m_frameCount->setText (text);
      }
      break;
    }

    case CMode::Timer :
    {
      if (m_ringtonePlayer->state () == QMediaPlayer::PlayingState)
      {
        m_ringtonePlayer->stop ();
      }

      CTimer* timer = static_cast<CTimer*>(ui->m_device->mode ());
      timer->restart ();
      break;
    }

    default :
      break;
  }
}

void CMainWindow::stopwatch ()
{
  updateStopwatchButons ();
  if (ui->m_device->nixieTubeNumber () != 6)
  {
    ui->m_device->setNixieTubeNumber (6);
    ui->m_device->changeVbo ();
  }

  ui->m_device->setType (CMode::Stopwatch);
}
