#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../vmode/thermometer.hpp"

void CMainWindow::thermometer ()
{
  updateClockButtons ();
  if (ui->m_device->nixieTubeNumber () != 3)
  {
    ui->m_device->setNixieTubeNumber (3);
    ui->m_device->changeVbo ();
  }

  ui->m_device->setType (CMode::Thermometer, m_celcius);
  auto mode = static_cast<CThermometer*>(ui->m_device->mode ());
  mode->setSensor (m_ambiantTemperatureSensor);
  ui->m_device->updateTubes ();
}

void CMainWindow::clock ()
{
  m_countClockChanged = 0;
  updateClockButtons ();
  if (ui->m_device->nixieTubeNumber () != m_cClockTubes)
  {
    ui->m_device->setNixieTubeNumber (m_cClockTubes);
    ui->m_device->changeVbo ();
  }

  ui->m_device->setType (CMode::Clock, m_h24);
}

void CMainWindow::fastCounter ()
{
  setSimpleMode (CMode::FastCounter);
}

void CMainWindow::digitTest ()
{
  setSimpleMode (CMode::DigitTest);
}
