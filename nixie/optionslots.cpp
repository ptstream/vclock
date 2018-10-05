#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "displaymanager.hpp"
#include "../vmode/clock.hpp"
#include "../vmode/thermometer.hpp"

void CMainWindow::display ()
{
  CDisplayManager mgr (ui->m_device->display (), ui->m_device->fontFamily (), this);
  int ret = mgr.exec ();
  if (ret == QDialog::Accepted)
  {
    CNixie::EDisplay type = mgr.displayType ();
    ui->m_device->setDisplay (type);
    QString const & fontFamily = mgr.fontFamily ();
    if (fontFamily != ui->m_device->fontFamily ())
    {
      ui->m_device->setFontFamily (fontFamily);
      setFontFamily (fontFamily);
    }
  }
}

void CMainWindow::tubeNumber ()
{
  CMode::EType type = ui->m_device->type ();
  if (type == CMode::Clock || type == CMode::FastCounter)
  {
    int n = ui->m_device->nixieTubeNumber () == 4 ? 6 : 4;
    ui->m_device->setNixieTubeNumber (n);
    m_cClockTubes = n;
    ui->m_device->changeVbo ();
  }
}

void CMainWindow::degree ()
{
  m_celcius = !m_celcius;
  if (ui->m_device->type () == CMode::Thermometer)
  {
    static_cast<CThermometer*>(ui->m_device->mode ())->setCelcius (m_celcius);
    ui->m_device->updateTubes ();
  }

  updateWeather ();
}

void CMainWindow::hour ()
{
  m_h24 = !m_h24;
  if (ui->m_device->type () == CMode::Clock)
  {
    static_cast<CClock*>(ui->m_device->mode ())->setH24 (m_h24);
  }
}

void CMainWindow::date ()
{
  ui->m_date->setHidden (!ui->m_date->isHidden ());
}

void CMainWindow::showHideTable ()
{
  ui->m_device->reversePartCode (CNixie::PCTable);
  update ();
}

void CMainWindow::showHidePencil ()
{
  ui->m_device->reversePartCode (CNixie::PCPencil);
  update ();
}

void CMainWindow::stopStartDiaporama ()
{
  ui->m_device->stopStartDiaporama ();
}

void CMainWindow::forceReleaseBackgroungUniform ()
{
  ui->m_device->forceReleaseBackgroungUniform ();
}

void CMainWindow::move ()
{
  ui->m_device->setInMotion (!ui->m_device->inMotion ());
}


