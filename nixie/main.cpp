#include "mainwindow.hpp"
#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>

int main (int argc, char *argv[])
{
  QApplication a (argc, argv);

  // Read initialization of openGL from nixie.ini
  CMainWindow::SOpenGlParams params;
  CMainWindow::openGLConfig (params);

  // Global initialization of openGL.
  QSurfaceFormat fmt;
  fmt.setDepthBufferSize (params.m_depthBufferSize);
  fmt.setSamples (params.m_samples);
  QSurfaceFormat::setDefaultFormat (fmt);

  CMainWindow w;
  if (!params.m_showTitle)
  {
    w.setWindowFlags (Qt::Window | Qt::FramelessWindowHint);
  }

  if (params.m_maxWindowSize)
  {
    w.setWindowState (Qt::WindowMaximized);
  }
  else
  {
    w.restoreGeometry (params.m_rc, params.m_ws);
  }

  QIcon icon (":/icons/nixie.ico");
  w.setWindowIcon (icon);
  w.show ();
  return a.exec ();
}
