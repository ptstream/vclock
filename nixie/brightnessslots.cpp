#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../vtools/brightness.hpp"
#include "../vtools/brightnessinterface.hpp"
#include "../vtools/helper.hpp"

void CMainWindow::autoBrightness ()
{
  if (m_brightness != nullptr)
  {
    m_brightness->setEnabled (!m_brightness->isEnabled ());
  }
}

void CMainWindow::brightness0 ()
{
  setBrightness (0);
}

void CMainWindow::brightness1 ()
{
  setBrightness (1);
}

void CMainWindow::brightness2 ()
{
  setBrightness (2);
}

void CMainWindow::brightness3 ()
{
  setBrightness (3);
}

void CMainWindow::brightness4 ()
{
  setBrightness (4);
}

void CMainWindow::on_m_luminosity_valueChanged (int value)
{
  setBrightness (value);
}

void CMainWindow::brightnessChanged (float value)
{
  float brightness = value == 0.0f ? m_lowBrightness : m_hightBrightness;
  if (m_brightnessPlugin != nullptr)
  {
    m_brightnessPlugin->setValue (brightness);
  }
}
