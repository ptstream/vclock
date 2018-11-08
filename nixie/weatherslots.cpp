#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "localisation.hpp"
#include "../vmode/thermometer.hpp"
#include "../vtools/weatherinterface.hpp"
#include "../vtools/externalipinterface.hpp"
#include "../vtools/iplocationinterface.hpp"
#include "../vtools/helper.hpp"
#include <QDesktopServices>

void CMainWindow::initLocation ()
{
  QByteArray pluginVersion;
  QObject*   plugin = ::loadPlugin (m_ipExternalUrl, &pluginVersion);
  if (plugin != nullptr)
  {
    CExternalIpInterface* eii = qobject_cast<CExternalIpInterface*>(plugin);
    if (eii != nullptr && eii->isCompatible (pluginVersion))
    {
      QHostAddress addr = eii->data (m_nam);
      delete eii;
      plugin = ::loadPlugin (m_locationUrl, &pluginVersion);
      if (plugin != nullptr)
      {
        CIpLocationInterface* ili = qobject_cast<CIpLocationInterface*>(plugin);
        if (ili != nullptr && ili->isCompatible (pluginVersion))
        {
          ili->setExternalAddress (addr);
          QPair<QString, QString> loc = ili->data (m_nam);
          m_weather->setTown (loc.first);
          m_weather->setCountry (loc.second);
          delete ili;
        }
      }
    }
  }
}

void CMainWindow::on_m_weatherLogo_clicked ()
{
  QUrl url = m_weather->advertising ();
  QDesktopServices::openUrl (url);
}

void CMainWindow::updateWeather ()
{
  int const maxRetries = 4;
  if (m_weather != nullptr)
  {
    QApplication::setOverrideCursor (Qt::WaitCursor);
    if (!m_weather->hasValidLocation ())
    {
      QString w8 = tr ("Searching for location");
      ui->m_w8->setText (w8);
      initLocation ();
      ui->m_w8->clear ();
    }

    if (!m_weather->town ().isEmpty () && !m_weather->country ().isEmpty ())
    {
      QString w8 = tr ("Searching for ") + m_weather->town () + " ("  + m_weather->country () + ") -> " +
                   QString::number (m_weatherRetry + 1);
      ui->m_w8->setText (w8);
      QVector<CForecast> forecasts = m_weather->data (m_nam);
      if (!forecasts.isEmpty ())
      {
        auto degree = [this] (float temp) -> QString
        {
          if (!m_celcius)
          {
            temp = CThermometer::toFahrenheit (temp);
          }

          return QString::number (qRound (temp)) + "°";
        };

        if (ui->m_w9->isHidden ())
        {
          weatherSetHidden (false);
        }

        CForecast const & forecast = forecasts[0];
        ui->m_w3->setText (degree (forecast.temp ()));
        ui->m_w5->setText (degree (forecast.tempMax ()));
        ui->m_w7->setText (degree (forecast.tempMin ()));

        QString text   = forecast.text ().toLower ();
        QString trText = QCoreApplication::translate ("yahooweather", text.toLatin1 ());
        if (trText == text)
        {
          trText = forecast.text ();
        }

        ui->m_w8->setText (trText);
        ui->m_w9->setPixmap (m_weather->pixmap (forecast.code ()));

        QString line;
        QString texts ("<!doctype html><table style=\"height:10%\">");
        QDate   date = QDate::currentDate ();
        for (int i = 1, count = m_weather->count (); i < count; ++i)
        {
          CForecast const & forecast = forecasts[i];
          text                       = forecast.text ().toLower ();
          trText                     = QCoreApplication::translate ("yahooweather", text.toLatin1 ());
          if (trText == text)
          {
            trText = forecast.text ();
          }

          date        = date.addDays (1);
          QString day = date.toString("dddd");
          line        = QString ("<tr><th>%1</th><th>%2</th><th><img src=\":/yahooicons/%3.png\"></th><th>%4</th><th>%5</th></tr>").
                        arg (day, trText).arg (forecast.code ()).arg (degree (forecast.tempMin ()), degree (forecast.tempMax ()));
          texts += line;
        }

        texts += "</table>";
        ui->m_forecasts->setText (texts);
        m_weatherTimer.setInterval (m_weatherInterval);
      }
      else
      {
        ++m_weatherRetry;
        if (m_weatherRetry == maxRetries)
        {
          m_weatherTimer.stop ();
          delete m_weather;
          m_weather = nullptr;
        }
      }
    }
    else
    {
      m_weatherTimer.stop ();
      ui->m_w8->clear ();
    }

    if (m_weatherRetry == maxRetries || m_weather->town ().isEmpty ())
    {
      ui->m_w8->clear ();
      ui->m_w4->setHidden (true);
      ui->m_w6->setHidden (true);
    }


    QApplication::restoreOverrideCursor ();
  }
}

void CMainWindow::localisation ()
{
  CLocalisation loc (m_weather->town (), m_weather->country (), this);
  int ret = loc.exec ();
  if (ret == 1)
  {
    m_weather->setTown (loc.town ().trimmed ());
    m_weather->setCountry (loc.zipCode ().trimmed ());
    if (m_weather->hasValidLocation ())
    {
      weatherSetHidden (false);
      weather ();
    }
  }
}

void CMainWindow::weather ()
{
  bool hide = !ui->m_w9->isHidden ();
  weatherSetHidden (hide);
  if (!hide)
  {
    startWeather ();
  }
}

void CMainWindow::forecasts ()
{
  if (ui->m_w9->isHidden ())
  {
    weather ();
  }

  bool hide = !ui->m_forecasts->isHidden ();
  ui->m_forecasts->setHidden (hide);
  if (!hide)
  {
    if (m_currentAnodeBrightness > CNixie::BLow)
    {
      ui->m_device->setAnodeBrightness (CNixie::BLow);
    }
  }
  else
  {
    ui->m_device->setAnodeBrightness (m_currentAnodeBrightness);
  }
}
