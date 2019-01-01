#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../vmode/timer.hpp"
#include "../vtools/alarm.hpp"
#include "../vtools/brightness.hpp"
#include "../vtools/weatherinterface.hpp"
#include "../vtools/tempsensorinterface.hpp"
#include "../vtools/brightnessinterface.hpp"
#include "../vtools/webradio.hpp"
#include "../vtools/nam.hpp"
#include "../vtools/helper.hpp"
#include <QPainter>
#include <QAction>
#include <cstring>
#include <cmath>

QColor CMainWindow::m_primaryColor = QColor (255, 128, 25);

CMainWindow::CMainWindow (QWidget *parent) : QMainWindow (parent),  ui (new Ui::CMainWindow)
{
  ui->setupUi (this);
  std::memset (m_options, 0, sizeof (m_options));
  ui->m_device->setPrimaryColor (m_primaryColor);
  m_nam = new QNetworkAccessManager (this);
  resetControls ();
  applyConfig ();
  setMainMenu ();
  setSleepingMenu ();
  m_sleepingTimer.setSingleShot (true);
  setWidgetColor ();
  createRingtonePlayer ();
  connect (ui->m_device, &CNixie::frameCount, this, &CMainWindow::frameCount);
  connect (ui->m_device, &CNixie::clockChanged, this, &CMainWindow::clockChanged);
  connect (ui->m_device, &CNixie::glWidgetClicked, this, &CMainWindow::glWidgetClicked, Qt::QueuedConnection);
  connect (&m_weatherTimer, &QTimer::timeout, this, &CMainWindow::updateWeather);
  connect (&m_ringtoneVolumeTimer, &QTimer::timeout, this, &CMainWindow::ringtoneIncreaseVolume);
  connect (&m_sleepingTimer, &QTimer::timeout, this, &CMainWindow::sleepTimeout);
  if (m_brightness != nullptr)
  {
    connect (m_brightness, &CBrightness::changed, this, &CMainWindow::brightnessChanged);
  }

  QIcon icon;
  icon.addPixmap (QPixmap (":/icons/sleepingOff.png"), QIcon::Normal);
  icon.addPixmap (QPixmap (":/icons/sleepingDisable.png"), QIcon::Disabled);
  ui->m_sleeping->setIcon (icon);

  // This part is important for Windows OpenGL. It is not possible to allocate VBO in this constructor.
  connect (&m_startupTimer, &QTimer::timeout, this, &CMainWindow::digitTest);
  m_startupTimer.setSingleShot (true);
  m_startupTimer.start (250);
  //  QApplication::setOverrideCursor (Qt::BlankCursor);
}

CMainWindow::~CMainWindow ()
{
  for (CAlarm* alarm : m_alarms)
  {
    delete alarm;
  }

  CMode* mode = ui->m_device->mode ();
  for (CTimer* timer : m_timers)
  {
    if (timer == mode)
    {
      ui->m_device->setType (CMode::LastType);
    }

    delete timer;
  }

  delete m_ambiantTemperatureSensor;
  delete m_weather;
  delete ui;
}

void CMainWindow::resetControls (QLayout* layout)
{
  int cItems = layout->count ();
  for (int iItem = 0; iItem < cItems; ++iItem)
  {
    QLayoutItem* item   = layout->itemAt (iItem);
    QWidget*     widget = item->widget ();
    if (widget != nullptr && widget != ui->m_mainMenu && widget != ui->m_date && widget != ui->m_frameCount)
    {
      widget->setHidden (true);
    }
    else
    {
      QLayout* childLayout = item->layout ();
      if (childLayout != nullptr)
      {
        resetControls (childLayout);
      }
    }
  }
}

void CMainWindow::resetControls ()
{
  ui->m_frameCount->clear ();
  ui->m_swStartPause->setIcon (QIcon (QString (":/icons/play.png")));
  ui->m_swTourReset->setIcon (QIcon (QString (":/icons/reset.png")));
  ui->m_forecasts->setHidden (true);
  QLayout* layouts[] = { ui->m_headerLayout, ui->m_line2Layout, ui->m_footerLayout, ui->m_luminosityLayout };
  for (QLayout* layout : layouts)
  {
    resetControls (layout);
  }
}

void CMainWindow::setWidgetColor ()
{
  // Change color of texts
  QList<QLabel*> labels = findChildren<QLabel*> ();
  if (!labels.isEmpty ())
  {
    QPalette palette = labels.first ()->palette ();
    palette.setColor (QPalette::WindowText, m_primaryColor);
    for (QLabel* label : labels)
    {
      label->setPalette (palette);
    }
  }

  // Change color of QSlider.
  QString htmlColor = QString ("#%1%2%3").arg (m_primaryColor.red (), 2, 16, QLatin1Char ('0'))
                                         .arg (m_primaryColor.green (), 2, 16, QLatin1Char ('0'))
                                         .arg (m_primaryColor.blue (), 2, 16, QLatin1Char ('0'));
  QString styleSheet = QString ("\
QSlider::groove { \
background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #000, stop:1 %1);\
height: 12px; \
border-radius: 6px;\
}\
QSlider::handle { \
background-color: %1; \
width: 30px; \
height: 30px; \
margin-top: -5px; \
margin-bottom: -5px; \
border-radius: 10px; \
}\
QSlider::handle:hover { \
border-radius: 10px;\
}").arg (htmlColor);

  ui->m_volume->setStyleSheet (styleSheet);
  styleSheet = QString ("\
QSlider::groove { \
background: qlineargradient(x1:0, y1:1, x2:0, y2:0, stop:0 #000, stop:1 %1);\
border: 0px solid %1; \
width: 12px; \
border-radius: 6px;\
}\
QSlider::handle { \
background-color: %1; \
height: 30px; \
width: 30px; \
margin-left: -5px; \
margin-right: -5px; \
border-radius: 10px; \
}").arg (htmlColor);

  ui->m_luminosity->setStyleSheet (styleSheet);
}

void CMainWindow::setBrightness (int index)
{
  if (index < 0)
  {
    index = 0;
  }
  else
  {
    int const indexMax = static_cast<int>(sizeof (m_lightAttenations) / sizeof (float)) - 1;
    if (index > indexMax)
    {
      index = indexMax;
    }
  }

  ui->m_device->setLightAttenuation (m_lightAttenations[index]);
}

float CMainWindow::brightness () const
{
  float value = 1.0f;
  if (m_brightnessPlugin != nullptr)
  {
    float v = m_brightnessPlugin->value ();
    if (std::fabs (v - m_lowBrightness) < std::fabs (v - m_hightBrightness))
    {
      value = 0.0f;
    }
  }

  return value;
}

void CMainWindow::changeEvent (QEvent* event)
{
  if (event->type () == QEvent::LanguageChange)
  {
    ui->retranslateUi (this); // retranslate designer forms
  }

  QWidget::changeEvent (event); // remember to call base class implementation
}

void CMainWindow::closeEvent (QCloseEvent*)
{
  saveConfig ();
}

bool CMainWindow::isOnWeather (int x, int y) const
{
  QRect rc = ui->m_w0->isVisible () ? ui->m_w0->geometry () : ui->m_w2->geometry ();
  rc.setRight (ui->m_w8->geometry ().right ());
  return rc.contains (x, y);
}

bool CMainWindow::isOnDate (int x, int y) const
{
  QRect rc = ui->m_date->geometry ();
  return rc.contains (x, y);
}

bool CMainWindow::isOnPlayer (int x, int y) const
{
  return ui->m_device->clickedSection (x, y) == CNixie::Bottom;
}

bool CMainWindow::isOnLuminosity (int x, int y) const
{
  return ui->m_device->clickedSection (x, y) == CNixie::Right;
}

bool CMainWindow::isOnMode (int x, int y) const
{
  bool ok = ui->m_device->clickedSection (x, y) == CNixie::Left;
  if (ok)
  {
    QRect rect = ui->m_device->rect ();
    ok         = y > rect.height () / 2;
  }

  return ok;
}

bool CMainWindow::isOnModeDayNight (int x, int y) const
{
  bool ok = ui->m_device->clickedSection (x, y) == CNixie::Left;
  if (ok)
  {
    QRect rect = ui->m_device->rect ();
    ok         = y < rect.height () / 2;
  }

  return ok;
}

void CMainWindow::startWeather ()
{
  m_weatherTimer.setInterval (500);
  m_weatherTimer.start ();
}

void CMainWindow::weatherSetHidden (bool hide)
{
  QWidget *weatherWidgets[] = { ui->m_w0, ui->m_w1, ui->m_w2, ui->m_w3 ,ui->m_w4 ,ui->m_w5,
                                ui->m_w6, ui->m_w7, ui->m_w8, ui->m_w9, ui->m_weatherLogo };
  for (QWidget* widget : weatherWidgets)
  {
    widget->setHidden (hide);
  }
}

void CMainWindow::setHidden (QList<QToolButton*> const & tbs, bool hidden)
{
  for (QWidget* tb : tbs)
  {
    tb->setHidden (hidden);
  }
}

void CMainWindow::updateAlarmButtons ()
{
  QList<QToolButton*> tbs = { ui->m_a1, ui->m_a2, ui->m_a3 };
  setHidden (tbs, true);
  QColor    color = alarmIndexColor (m_primaryColor);
  for (int iAlarm = 0, cAlarms = m_alarms.size (); iAlarm < cAlarms; ++iAlarm)
  {
    if (m_alarms[iAlarm]->isEnabled ())
    {
      for (QToolButton* tb : tbs)
      {
        if (tb->isHidden ())
        {
          QPixmap pxm (":/icons/alarm.png");
          pxm = addIndex (pxm, iAlarm + 1, color);
          tb->setIcon (pxm);
          tb->setVisible (true);
          m_alarms[iAlarm]->setWidget (tb);
          break;
        }
      }
    }
  }
}

void CMainWindow::updateStopwatchButons ()
{
  ui->m_frameCount->clear ();
  ui->m_forecasts->setHidden (true);
  setHidden ( { ui->m_swStartPause, ui->m_swTourReset }, false);
}

void CMainWindow::updateClockButtons ()
{
  ui->m_frameCount->clear ();
  ui->m_forecasts->setHidden (true);
  setHidden ( { ui->m_swStartPause, ui->m_swTourReset }, true);
}

QColor CMainWindow::alarmIndexColor (QColor color)
{
  color.setRed (std::min (color.red () * 2, 255));
  color.setGreen (std::min (color.green () * 2, 255));
  color.setBlue (std::min (color.blue () * 2, 255));
  return color;
}

void CMainWindow::launchAlarms ()
{
  int cAlarms = m_alarms.size ();
  if (cAlarms == 0)
  {
    delete m_ringtonePlayer;
    m_ringtonePlayer = nullptr;
    ui->m_device->setDynamicRotationEnabled (true);
  }
  else
  {
    createRingtonePlayer ();
    for (int iAlarm = 0; iAlarm < cAlarms; ++iAlarm)
    {
      CAlarm* alarm = m_alarms[iAlarm];
      if (alarm->isEnabled ())
      {
        connect (alarm, &CAlarm::stopped, this, &CMainWindow::alarmStopped);
        connect (alarm, &CAlarm::restarted, this, &CMainWindow::alarmRestarted);
        if (alarm->repeatCount () != 0)
        {
          connect (alarm, &CAlarm::repeatStarted, this, &CMainWindow::alarmRepeatStarted);
        }

        alarm->start ();
      }
    }
  }

  updateAlarmButtons ();
}

void CMainWindow::createRingtonePlayer ()
{
  if (m_ringtonePlayer == nullptr)
  {
    m_ringtoneDuration = 4 * CAlarm::initialRepeatCount () * CAlarm::repeatInterval ();
    m_ringtonePlayer   = new QMediaPlayer (this);
#ifndef QTMULTIMEDIA_DEFINED
    m_ringtonePlayer->setPlayerComs (m_webRadios.playerComs ());
    m_ringtonePlayer->setMixerProgram (m_webRadios.mixerProgram ());
    connect (m_ringtonePlayer, &QMediaPlayer::trackEnded, this, &CMainWindow::ringtoneFinished);
#else
    connect (m_ringtonePlayer, &QMediaPlayer::stateChanged, this, &CMainWindow::ringtoneStateChanged);
#endif
  }
}

void CMainWindow::alarmClicked (QObject* o)
{
  auto tb = static_cast<QToolButton*>(o);
  for (CAlarm* alarm : m_alarms)
  {
    if (alarm->widget () == tb)
    {
      if (alarm->repeatTimerActive ())
      {
        m_ringtoneVolumeTimer.stop ();
        m_ringtonePlayer->stop ();
        ui->m_frameCount->clear ();
        m_alarms.removeOne (alarm);
        delete alarm;
        updateAlarmButtons ();
#ifdef QTMULTIMEDIA_DEFINED
        disconnect (m_ringtonePlayer, &QMediaPlayer::stateChanged, nullptr, nullptr);
#else
        disconnect (m_ringtonePlayer, &QMediaPlayer::trackEnded, nullptr, nullptr);
#endif
        ui->m_device->setDynamicRotationEnabled (true);
      }
      else
      {
        QToolButton* alarmButtons[] = { ui->m_a1, ui->m_a2, ui->m_a3 };
        for (QToolButton* alarmButton : alarmButtons)
        {
          if (alarmButton != tb)
          {
            alarmButton->setChecked (false);
          }
        }

        if (tb->isChecked ())
        {
          QString text = alarm->toString ();
          ui->m_frameCount->setText (text);
        }
        else
        {
          ui->m_frameCount->clear ();
        }
      }
      break;
    }
  }
}

void CMainWindow::updateTimerButtons ()
{
  QColor color = alarmIndexColor (m_primaryColor);
  updateClockButtons ();
  QList<QToolButton*> tbs = { ui->m_t1, ui->m_t2, ui->m_t3 };
  setHidden (tbs, true);
  int iButton = 0;
  for (CTimer* timer : m_timers)
  {
    timer->setWidget (nullptr);
    if (timer->isEnabled ())
    {
      QToolButton* tb = tbs[iButton];
      timer->setWidget (tb);
      if (ui->m_device->mode () == timer)
      {
        addActive (tb, color);
      }
      else
      {
        tb->setIcon (QIcon (":/icons/sandglass.png"));
      }

      tb->setVisible (true);
      ++iButton;
      if (iButton >= 3)
      {
        break;
      }
    }
  }

  bool hide = !(ui->m_t1->isVisible () || ui->m_t2->isVisible () || ui->m_t3->isVisible ());
  ui->m_frameCount->clear ();
  ui->m_swStartPause->setHidden (hide);
  ui->m_swTourReset->setHidden (hide);
  ui->m_swStartPause->setIcon (QIcon (":/icons/pause.png"));
  if (hide)
  {
    auto type = static_cast<CMode::EType>(m_prevType & 0xff);
    bool data = type == CMode::Clock ? m_h24 : m_celcius;
    ui->m_device->setType (type, data);
    ui->m_device->setNixieTubeNumber ((m_prevType >> 8) & 0xff);
  }
}

void CMainWindow::setMode (CMode::EType type)
{
  switch (type)
  {
    case CMode::Thermometer :
      thermometer ();
      break;

    case CMode::Stopwatch :
      stopwatch ();
      break;

    default :
      clock ();
      break;
  }
}

void CMainWindow::timerClicked (QObject* sender)
{
  for (CTimer* timer : m_timers)
  {
    if (timer->isActive () && timer->isEnabled ())
    {
      auto tb = static_cast<QToolButton*>(timer->widget ());
      if (sender == tb)
      {
        if (ui->m_device->mode () == timer)
        {
          sender = nullptr;
          timer->stop ();
          timer->setEnabled (false);
          if (m_ringtonePlayer != nullptr && timer->digits () == 0)
          {
            m_ringtoneVolumeTimer.stop ();
            m_ringtonePlayer->stop ();
          }
        }
        else
        {
          ui->m_device->setType (CMode::Timer, QVariant::fromValue (timer));
        }
        break;
      }
    }
  }

  if (sender == nullptr && !m_timers.isEmpty ())
  {
    for (CTimer* timer : m_timers)
    {
      if (timer->isEnabled ())
      {
        ui->m_device->setType (CMode::Timer, QVariant::fromValue (timer));
        break;
      }
    }
  }

  updateTimerButtons ();
}

void CMainWindow::updateSleeping ()
{
  char const * iconName = m_sleepingTimer.isActive () ? "sleepingOn" : "sleepingOff";
  QIcon        icon     = ui->m_sleeping->icon ();
  icon.addPixmap (QPixmap (QString (":/icons/%1.png").arg (iconName)), QIcon::Normal);
  ui->m_sleeping->setIcon (icon);
  ui->m_sleeping->setEnabled (m_webRadios.isPlaying ());
}

int CMainWindow::sleepingInterval (QAction* action)
{
  int     interval = 0;
  QString text     = action->text ();
  if (text[0].isDigit ())
  {
    interval = text[0].digitValue ();
  }

  if (text[1].isDigit ())
  {
    interval *= 10;
    interval += text[1].digitValue ();
  }

  return interval;
}

QPixmap CMainWindow::addIndex (QPixmap const & pxm, unsigned index, QColor const & color, bool repeat)
{
  QString  text = index < 32 ? QString::number (index) : QString (QChar (index));
  int      hPxm = pxm.height ();

  QPixmap  pixmap (pxm);
  QPainter painter (&pixmap);

  QFont font = painter.font ();
  font.setBold (true);
  font.setPixelSize (hPxm / 2);

  QFontMetrics fm (font);
  painter.setFont (font);
  int h = repeat ? hPxm : fm.ascent ();
  int w = fm.width (text);

  painter.setBrush (color);
  painter.setPen (color);
  painter.drawText (pxm.width () - w, h, text);
  return pixmap;
}

QPixmap CMainWindow::addActive (QPixmap const & pxm, QColor const & color)
{
  return addIndex (pxm, 'A', color);
}

void CMainWindow::addActive (QToolButton* tb, QColor const & color)
{
  QPixmap pxm = addActive (QPixmap (":/icons/sandglass.png"), color);
  tb->setIcon (QIcon (pxm));
}

void CMainWindow::setFontFamily (QString const & family)
{
  QLabel* labels[] = { ui->m_w1,
                       ui->m_w3,
                       ui->m_w5,
                       ui->m_w7,
                       ui->m_w8,
                       ui->m_date,
                       ui->m_station,
                       ui->m_forecasts,
                       ui->m_frameCount,
                     };

  for (QLabel* label : labels)
  {
    QFont font = label->font ();
    font.setFamily (family);
    label->setFont (font);
  }
}

void CMainWindow::setSimpleMode (CMode::EType type)
{
  updateClockButtons ();
  int cTubes = ui->m_device->nixieTubeNumber ();
  if (cTubes != 4 && cTubes != 6)
  {
    ui->m_device->setNixieTubeNumber (m_cClockTubes);
    ui->m_device->changeVbo ();
  }

  ui->m_device->setType (type);
  ui->m_frameCount->clear ();
}


