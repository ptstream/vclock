#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "timermanager.hpp"
#include "../vmode/timer.hpp"

void CMainWindow::timer ()
{
  CMode*        mode = ui->m_device->mode ();
  CMode::EType  type = mode->type ();
  CTimerManager tm (m_timers, this);
  int ret = tm.exec ();
  if (ret == QDialog::Accepted)
  {
    if (type != CMode::Timer)
    {
      m_prevType = type | (ui->m_device->nixieTubeNumber () << 8);
    }

    bool sixDigits = false;
    for (CTimer* timer : m_timers)
    {
      if (timer->isEnabled () && !timer->isActive ())
      {
        connect (timer, &CTimer::timeout, this, &CMainWindow::ring);
        timer->swapState ();
      }

      sixDigits |= timer->isActive ();
    }

    if (sixDigits && ui->m_device->nixieTubeNumber () != 6)
    {
      ui->m_device->setNixieTubeNumber (6);
      ui->m_device->changeVbo ();
    }

    if (!m_timers.isEmpty ())
    {
      for (CTimer* timer : m_timers)
      {
        if (timer->isEnabled ())
        {
          ui->m_device->setType (CMode::Timer, QVariant::fromValue (m_timers.first ()));
        }
        break;
      }
    }

    updateTimerButtons ();
  }
}

void CMainWindow::on_m_t1_clicked ()
{
  timerClicked (sender ());
}

void CMainWindow::on_m_t2_clicked ()
{
  timerClicked (sender ());
}

void CMainWindow::on_m_t3_clicked ()
{
  timerClicked (sender ());
}

