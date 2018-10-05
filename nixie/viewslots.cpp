#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void CMainWindow::frontView ()
{
  ui->m_device->setXRot (0);
  ui->m_device->setYRot (0);
  ui->m_device->setZRot (0);
  ui->m_device->update ();
}

void CMainWindow::topView ()
{
  ui->m_device->setXRot (90);
  ui->m_device->setYRot (0);
  ui->m_device->setZRot (0);
  ui->m_device->update ();
}

void CMainWindow::leftView ()
{
  ui->m_device->setXRot (0);
  ui->m_device->setYRot (90);
  ui->m_device->setZRot (0);
  ui->m_device->update ();
}

void CMainWindow::rightView ()
{
  ui->m_device->setXRot (0);
  ui->m_device->setYRot (-90);
  ui->m_device->setZRot (0);
  ui->m_device->update();
}

void CMainWindow::perspective ()
{
  ui->m_device->setXRot (10);
  ui->m_device->setYRot (8);
  ui->m_device->setZRot (0);
  ui->m_device->update ();
}
