#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../vtools/weatherinterface.hpp"
#include "../vtools/brightness.hpp"
#include "../vtools/helper.hpp"
#include <QMenu>
#include <QAction>
#include <cmath>

static void changeFontSize (QMenu* menu, float coef)
{
  if (coef != 1.0f)
  {
    QFont font = menu->font ();
    float size = font.pointSizeF () * coef;
    font.setPointSizeF (size);
    menu->setFont (font);
  }
}

static QMenu* addMenu (QMenu* parent, QString const & title, float coef)
{
  QMenu* menu = parent->addMenu (title);
  changeFontSize (menu, coef);
  return menu;
}

void CMainWindow::setMainMenu ()
{
  auto menu = new QMenu (ui->m_mainMenu);
  changeFontSize (menu, m_menuFontCoef);
  ui->m_mainMenu->setMenu (menu);

  QMenu* viewMenu = addMenu (menu, tr ("View"), m_menuFontCoef);
  viewMenu->addAction (tr ("Front view"), this, &CMainWindow::frontView);
  viewMenu->addAction (tr ("Top view"), this, &CMainWindow::topView);
  viewMenu->addAction (tr ("Left view"), this, &CMainWindow::leftView);
  viewMenu->addAction (tr ("Right view"), this, &CMainWindow::rightView);
  viewMenu->addAction (tr ("Perspective"), this, &CMainWindow::perspective);

  QMenu* modeMenu = addMenu (menu, tr ("Mode"), m_menuFontCoef);
  modeMenu->addAction (tr ("Clock"), this, &CMainWindow::clock);
  modeMenu->addAction (tr ("Stopwatch"), this, &CMainWindow::stopwatch);
  modeMenu->addAction (tr ("Timer"), this, &CMainWindow::timer);
  modeMenu->addAction (tr ("Thermometer"), this, &CMainWindow::thermometer);
  modeMenu->addAction (tr ("Fast counter"), this, &CMainWindow::fastCounter);
  modeMenu->addAction (tr ("Digit test"), this, &CMainWindow::digitTest);

  menu->addSeparator ();
  QMenu* optionsMenu = addMenu (menu, tr ("Options"), m_menuFontCoef);
  m_options[Display] = optionsMenu->addAction (tr ("Display"), this, &CMainWindow::display);
  optionsMenu->addSeparator ();
  m_options[TubeNumber] = optionsMenu->addAction (tr ("6 tubes"), this, &CMainWindow::tubeNumber);
  m_options[Hour]       = optionsMenu->addAction (tr ("24 hours"), this, &CMainWindow::hour);
  m_options[Degree]     = optionsMenu->addAction (tr ("Celcius"), this, &CMainWindow::degree);
  optionsMenu->addSeparator ();
  QMenu* optionsSubmenu1 = addMenu (optionsMenu, tr ("Show/Hide"), m_menuFontCoef);
  m_options[Weather]     = optionsSubmenu1->addAction (tr ("Meteo"), this, &CMainWindow::weather);
  m_options[Forecasts]   = optionsSubmenu1->addAction (tr ("Forecasts"), this, &CMainWindow::forecasts);
  m_options[Date]        = optionsSubmenu1->addAction (tr ("Date"), this, &CMainWindow::date);
  m_options[Table]       = optionsSubmenu1->addAction (tr("Table"), this, &CMainWindow::showHideTable);
  m_options[Pencil]      = optionsSubmenu1->addAction (tr ("Pencil"), this, &CMainWindow::showHidePencil);
  m_options[Player]      = optionsSubmenu1->addAction (tr("Player"), this, &CMainWindow::showHidePlayer);
  optionsMenu->addSeparator ();
  m_options[Diaporama]              = optionsMenu->addAction (tr ("Stop diaporama"), this, &CMainWindow::stopStartDiaporama);
  m_options[BackgroundUniformColor] = optionsMenu->addAction (tr ("Uniform color"), this, &CMainWindow::forceReleaseBackgroungUniform);
  m_options[Move]                   = optionsMenu->addAction (tr ("Move"), this, &CMainWindow::move);

  menu->addSeparator ();
  QMenu* brightnessMenu = addMenu (menu, tr ("Brightness"), m_menuFontCoef);
  if (m_brightness != nullptr)
  {
    m_options[AutoBrigthness] = brightnessMenu->addAction (tr ("Automatic brightness"), this, &CMainWindow::autoBrightness);
    menu->addSeparator ();
  }

  m_options[HightBrigthness]       = brightnessMenu->addAction (tr ("Hight"), this, &CMainWindow::brightness0);
  m_options[AboveMediumBrigthness] = brightnessMenu->addAction (tr ("Above medium"), this, &CMainWindow::brightness1);
  m_options[MediumBrigthness]      = brightnessMenu->addAction (tr ("Medium"), this, &CMainWindow::brightness2);
  m_options[BelowMediumBrigthness] = brightnessMenu->addAction (tr ("below medium"), this, &CMainWindow::brightness3);
  m_options[LowBrigthness]         = brightnessMenu->addAction (tr ("Low"), this, &CMainWindow::brightness4);

  menu->addSeparator ();
  menu->addAction (tr ("Localisation"), this, &CMainWindow::localisation);
  menu->addAction (tr ("Alarms"), this, &CMainWindow::alarms);
  menu->addSeparator ();
  menu->addAction (tr ("About"), this, &CMainWindow::about);
  menu->addSeparator ();
  menu->addAction (tr ("Minimize"), this, &CMainWindow::showMinimized);
  menu->addAction (tr ("Close"), this, &CMainWindow::close);
  if (::touchScreenActivated ())
  {
    menu->addSeparator ();
    menu->addAction (tr ("Halt RPI"), this, &CMainWindow::haltRPI);
  }

  connect (menu, &QMenu::aboutToShow, this, &CMainWindow::aboutToShowMainMemu);
  for (QAction* action : m_options)
  {
    if (action != nullptr && action != m_options[0])
    {
      action->setCheckable (true);
    }
  }
}

void CMainWindow::aboutToShowMainMemu ()
{
  CMode::EType type = ui->m_device->type ();
  m_options[TubeNumber]->setEnabled (type == CMode::Clock || type == CMode::FastCounter);
  m_options[Hour]->setEnabled (type == CMode::Clock);

  bool enable = m_weather != nullptr && m_weather->hasValidLocation ();
  m_options[Weather]->setEnabled (enable);
  m_options[Forecasts]->setEnabled (enable);

  int cTubes = ui->m_device->nixieTubeNumber ();
  m_options[TubeNumber]->setEnabled (cTubes != 3);

  m_options[TubeNumber]->setChecked (cTubes == 6);
  m_options[Hour]->setChecked (m_h24);
  m_options[Date]->setChecked (ui->m_date->isVisible ());
  m_options[Table]->setChecked (ui->m_device->isVisiblePart (CNixie::PCTable));
  m_options[Pencil]->setChecked (ui->m_device->isVisiblePart (CNixie::PCPencil));
  m_options[Weather]->setChecked (ui->m_w9->isVisible ());
  m_options[Forecasts]->setChecked (ui->m_forecasts->isVisible ());
  m_options[Degree]->setChecked (m_celcius);
  m_options[Diaporama]->setChecked (!ui->m_device->isDiaporamaActive ());
  m_options[BackgroundUniformColor]->setChecked (ui->m_device->isBackgroungUniform ());
  m_options[Move]->setChecked (ui->m_device->inMotion ());
  m_options[Player]->setChecked (ui->m_station->isVisible ());
  bool enabled = ui->m_device->display () == CNixie::Nixie;
  m_options[Table]->setEnabled (enabled);
  m_options[Pencil]->setEnabled (enabled);

  float const epsilon     = 0.001f;
  float       attenuation = ui->m_device->lightAttenuation ();
  for (unsigned k = 0; k < sizeof (m_lightAttenations) / sizeof (float); ++k)
  {
    bool checked = std::fabs (m_lightAttenations[k] - attenuation) < epsilon;
    m_options[k + HightBrigthness]->setChecked (checked);
  }

  if (m_brightness != nullptr)
  {
    m_options[AutoBrigthness]->setChecked (m_brightness->isEnabled ());
  }

  QMenu* menu = ui->m_mainMenu->menu ();
  QList<QAction*> actions = menu->actions ();
  if (!actions.isEmpty ())
  {
    actions.first ()->setEnabled (enabled);
  }
}

void CMainWindow::setSleepingMenu ()
{
  auto menu = new QMenu (ui->m_sleeping);
  changeFontSize (menu, m_menuFontCoef);
  ui->m_sleeping->setMenu (menu);
  QString texts[] = { tr ("Disable"),
                      tr ("15mn"),
                      tr ("30mn"),
                      tr ("45mn"),
                      tr ("1h"),
                      tr ("2h"),
                      tr ("3h"),
                      tr ("4h"),
                    };

  bool first = true;
  for (QString const & text : texts)
  {
    QAction* action = menu->addAction (text, this, &CMainWindow::sleeping);
    action->setCheckable (true);
    if (first)
    {
      action->setChecked (true);
      first = false;
    }
  }

  connect (menu, &QMenu::aboutToShow, this, &CMainWindow::aboutToShowSleepMemu);
}

void CMainWindow::aboutToShowSleepMemu ()
{
  QMenu*          menu    = ui->m_sleeping->menu ();
  QList<QAction*> actions = menu->actions ();
  for (QAction* action : actions)
  {
    bool checked = m_sleepingInterval == sleepingInterval (action);
    action->setChecked (checked);
  }
}
