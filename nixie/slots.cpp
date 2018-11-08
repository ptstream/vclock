#include "mainwindow.hpp"
#include "about.hpp"
#include "ui_mainwindow.h"
#include "webradiosmanager.hpp"
#include "../vtools/webradio.hpp"
#include "../vmode/thermometer.hpp"
#include "../vtools/tempsensorinterface.hpp"
#include "../vtools/weatherinterface.hpp"
#include "../vtools/webradiosinterface.hpp"
#include "../vtools/alarm.hpp"
#include "../vtools/helper.hpp"
#include "../vtools/vtools_globals.h"
#include <QProcess>
#include <QDesktopServices>

void CMainWindow::frameCount (float count)
{
  ui->m_frameCount->setText (QString ("%1 fps").arg (count, 0, 'f', 1));
}

void CMainWindow::clockChanged ()
{
  CAlarm::setDateFormat (m_dateFormat);
  QDate date = QDate::currentDate ();
  ui->m_date->setText (QLocale::system ().toString (date, static_cast<QLocale::FormatType>(m_dateFormat)));
  CMode::EType type = ui->m_device->mode ()->type ();
  switch (type)
  {
    case CMode::Clock :
      if (m_ambiantTemperatureSensor != nullptr)
      {
        if (m_countClockChanged == 0)
        {
          if (ui->m_w0->isHidden ())
          {
            ui->m_w0->setHidden (false); // Show house icon
            ui->m_w1->setHidden (false); // Show temperature value
            ui->m_w2->setHidden (false); // Show panorama icon
          }

          float temp = m_ambiantTemperatureSensor->value ();
          if (temp != VTOOLS_INVALID_TEMPERATURE)
          {
            if (!m_celcius)
            {
              temp = CThermometer::toFahrenheit (temp);
            }

            QString text = QString::number (qRound (temp)) + "°";
            ui->m_w1->setText (text);
          }

          m_countClockChanged = CMode::interval (CMode::Thermometer) / CMode::interval (CMode::Clock);
        }
        else
        {
          --m_countClockChanged;
        }
      }
      else
      {
        if (!ui->m_w1->isHidden ())
        {
          ui->m_w0->setHidden (true); // Hide house icon
          ui->m_w1->setHidden (true); // Hide temperature value
          ui->m_w2->setHidden (true); // Hide panorama icon
        }
      }
      break;

    case CMode::Thermometer :
    {
      if (!ui->m_w0->isHidden ())
      {
        ui->m_w0->setHidden (true); // Hide house icon
        ui->m_w2->setHidden (true); // Hide panorama icon
      }

      QTime time = QTime::currentTime ();
      ui->m_w1->setText (QLocale::system ().toString (time, "hh:mm") + ' ');
      ui->m_w1->setHidden (false); // Show temperature value
      break;
    }

    default:
      break;
  }
}

void CMainWindow::haltRPI ()
{
  close ();
  QStringList args;
  args << "-c" << "sudo halt";
  QProcess::startDetached ("/bin/sh", args);
}

void CMainWindow::about ()
{
  QVector<QUrl> logos;
  logos << (m_weather != nullptr ? m_weather->advertising () : QUrl ());
  logos << (m_webRadiosProvider != nullptr ? m_webRadiosProvider->advertising () : QUrl ());
  logos << QUrl ("https://www.qt.io/");
  CAbout about (logos, this);
  about.exec ();
}

void CMainWindow::glWidgetClicked (int x, int y)
{
  if (!ui->m_device->hasDynamicRotationEnabled ())
  {
    if (m_ringtonePlayer != nullptr)
    {
      m_ringtoneVolumeTimer.stop ();
      m_ringtonePlayer->stop ();
    }
  }
  else if (isOnWeather (x, y))
  {
    if (!ui->m_w9->isHidden ())
    {
      forecasts ();
    }
  }
  else if (isOnDate (x, y))
  {
    m_dateFormat = !m_dateFormat;
    clockChanged ();
  }
  else if (isOnPlayer (x, y))
  {
    if (!ui->m_station->isHidden ())
    {
      QRect rc = ui->m_station->geometry ();
      if (rc.contains (x, y))
      {
        if (m_webRadiosProvider != nullptr)
        {
          CWebRadiosManager radios (m_webRadios, m_webRadiosProvider, m_nam, this);
          int ret = radios.exec ();
          if (ret == QDialog::Accepted)
          {
            int selectedRow = radios.selectedRow ();
            if (selectedRow != -1)
            {
              m_webRadios.setCurrentIndex (selectedRow);
              ui->m_station->setText (m_webRadios.currentName (m_stationNameLength));
              ui->m_logo->setPixmap (radios.selectedLogo ().pixmap (ui->m_logo->maximumWidth ()));
              if (!m_webRadios.isPlaying ())
              {
                on_m_audioPlay_clicked ();
              }

              if (radios.isTemp ())
              {
                m_webRadios.stations ().removeAt (selectedRow);
              }
            }
          }
        }
      }
      else
      {
        showHidePlayer ();
      }
    }
    else
    {
      showHidePlayer ();
    }
  }
  else if (isOnLuminosity (x, y))
  {
    ui->m_luminosity->setHidden (!ui->m_luminosity->isHidden ());
  }
  else if (isOnMode (x, y))
  {
    switch (ui->m_device->mode ()->type ())
    {
      case CMode::Clock :
        thermometer ();
        break;

      case CMode::Thermometer :
        clock ();
        break;

      default:
        break;
    }
  }
  else if (isOnModeDayNight (x, y))
  {
    float value = brightness () == 1.0f ? 0.0f : 1.0f;
    brightnessChanged (value);
  }
}

void CMainWindow::on_m_webRadiosLogo_clicked ()
{
  if (m_webRadiosProvider != nullptr)
  {
    QUrl url = m_webRadiosProvider->advertising ();
    QDesktopServices::openUrl (url);
  }
}
