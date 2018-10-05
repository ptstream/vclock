#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../vmode/timer.hpp"
#include "../vtools/webradio.hpp"
#include "../vtools/alarm.hpp"
#include "../vtools/brightness.hpp"
#include "../vtools/brightnessinterface.hpp"
#include "../vtools/brightnesssensorinterface.hpp"
#include "../vtools/weatherinterface.hpp"
#include "../vtools/tempsensorinterface.hpp"
#include "../vtools/webradiosinterface.hpp"
#include "../vtools/helper.hpp"
#include <QSettings>
#include <QDir>
#include <QDesktopWidget>
#include <QDateTime>

struct SConfigTubes
{
  CNixie::EDisplay  m_displayType            = CNixie::Nixie;
  int               m_cClockTubes            = 4;
  CMode::EType      m_modeType               = CMode::Clock;
  QColor            m_backgroundUniformColor = QColor (0, 0, 32, 255);
  QVector3D         m_moodLightPositions     = QVector3D (200.0, 0.0, 100.0);
  QVector3D         m_moodLightIntensities   = QVector3D (1.0, 1.0, 1.0);
  QString           m_backgroundTexture      = "$exe$/images/bkgnd.jpg";
  QString           m_aboutTexture           = "$exe$/images/about.png";
  QString           m_tableTexture           = "$exe$/images/table.jpg";
  QString           m_stlDir                 = "$exe$/stl";
  QString           m_diaporamaDir           = "$exe$/diaporama";
  CDiaporama::EMode m_diaporamaMode          = CDiaporama::FullImage;
  int               m_diaporamaInterval      = 4000; // seconds
  unsigned          m_partCode               = 0xffffffff & (~CNixie::PCTable) & (~CNixie::PCPencil);
  bool              m_forceBackgroungUniform = false;
  QString           m_fontFamily;
  float             m_menuFontCoef = 1.6f;
};

struct SConfigThermometer
{
  bool       m_celcius     = true;
  QString    m_sensorModel = "ds18b20";
  QByteArray m_sensorId; //    = "28-01430d07010c";
};

struct SConfigClock
{
  bool m_h24    = true;
  bool m_date   = true;
  int  m_format = 0;
};

struct SConfigPositions
{
  QVector3D m_motionIncs     = QVector3D (0.002, 0.002, 0.002);
  QVector3D m_motionMin      = QVector3D (-0.1, -0.1, -0.5);
  QVector3D m_motionMax      = QVector3D (0.1, 0.1, 0.3);
  int       m_motionInterval = 0;
  bool      m_motion         = false;
  QVector3D m_ps[7][6]       = { {},
                                 {},
                                 {},
                                 { QVector3D (-1.4, 0, 0), QVector3D (-0.14, 0, 0), QVector3D (1.4, 0, 0) },
                                 { QVector3D (-2.0, 0, 0), QVector3D (-0.8, 0, 0), QVector3D (0.8, 0, 0), QVector3D (2.0, 0, 0) },
                                 {},
                                 { QVector3D (-3.2, 0, 0), QVector3D (-2.0, 0, 0), QVector3D (-0.6, 0, 0), QVector3D (0.6, 0, 0), QVector3D (2.0, 0, 0), QVector3D (3.2, 0, 0) }
                               };
};

struct SConfigWeather
{
  QString m_provider      = "yahooweather";
  QString m_locationUrl   = "ip_api_com";
  QString m_ipExternalUrl = "api_ipify_org";
  int     m_interval      = 15; // Minutes
  QString m_town;
  QString m_country;
  bool m_activate = true;
};

struct SConfigWebradios
{
  QString           m_provider = "shoutcast";
  int               m_volume   = 20;
  bool              m_mp3      = true;
  int               m_index    = -1;
  QString           m_player   = "cvlc --quiet --extraintf=rc --repeat|add |quit|"; // Raspberry only
  QString           m_mixer    = "amixer"; // Raspberry only
  QVector<CStation> m_stations;
};

struct SConfigAlarms
{
  int            m_repeat     = 3;
  int            m_interval   = 10; // mn
  QString        m_ringtone   = "$exe$/ringtones/1.mp3";
  int            m_volumeMin  = 20;
  int            m_volumeMax  = 80;
  int            m_volumeStep = 5;
  QList<CAlarm*> m_alarms;
};

struct SConfigTimers
{
  QString m_ringtone = "$exe$/ringtones/2.mp3";
};

struct SConfigBrightness
{
  int     m_gpiopin = 0;
  bool    m_active  = true;
  float   m_hight   = 1.0f; // -> 255
  float   m_low     = 0.078f; // -> 20
  QString m_plugin; //  = "winbrightness or rpiblbrightness";
  QString m_sensor; //  = "rpigpiobrightness";
};

// Only -table -pencil is decoded.
static unsigned decodePartCode (QString const & s)
{
  unsigned partCode = 0xffffffff;
  if (!s.isEmpty () && s != "*")
  {
    bool ok;
    if (s.startsWith ("0x"))
    {
      unsigned c = s.toUInt (&ok, 16);
      if (ok)
      {
        partCode = c;
      }
    }
    else if (s[0].isDigit ())
    {
      unsigned c = s.toUInt (&ok, 16);
      if (ok)
      {
        partCode = c;
      }
    }
    else
    {
      QStringList parts = { "-table", "-pencil" };
      QStringList flags = s.split (' ');
      for (QString const & flag : flags)
      {
        QStringList::const_iterator it = std::find (parts.begin (), parts.end (), flag);
        switch (it - parts.begin ())
        {
          case 0 :
            partCode &= ~CNixie::PCTable;
            break;

          case 1 :
            partCode &= ~CNixie::PCPencil;
            break;
        }
      }
    }
  }

  return partCode;
}

static void decodeStation (CStation& s, QString const & settings)
{
  QStringList values = settings.split ('|');
  if (values.size () == 3)
  {
    s.setName (values[0].trimmed ());
    s.setLogo (values[1].trimmed ());
    s.setUrl (values[2].trimmed ());
  }
}

static QUrl ringtoneUrl (QString fileName)
{
  QUrl url;
  if (fileName.startsWith ("http"))
  {
    url = QUrl (fileName);
  }
  else
  {
    fileName = defaultFolder (fileName);
    if (QFile::exists (fileName))
    {
      url = QUrl::fromLocalFile (fileName);
    }
  }

  return url;
}

template<typename T>
static bool keyValue (QSettings& config, T& v, char const * key)
{
  bool exists = config.contains (key);
  if (exists)
  {
    v = static_cast<T>(config.value (key).toInt ());
  }

  return exists;
}

static bool keyValue (QSettings& config, float& v, char const * key)
{
  bool exists = config.contains (key);
  if (exists)
  {
    v = config.value (key).toFloat ();
  }

  return exists;
}

static bool keyValue (QSettings& config, QString& v, char const * key)
{
  bool exists = config.contains (key);
  if (exists)
  {
    v = config.value (key).toString ();
  }

  return exists;
}

static bool keyValue (QSettings& config, bool& v, char const * key)
{
  bool exists = config.contains (key);
  if (exists)
  {
    v = config.value (key).toBool ();
  }

  return exists;
}

static void pluginsFromKernel (QString& brightnessPlugin, QString& brightnessSensor)
{
  QString kernel = QSysInfo::productType ();
  if (kernel == "windows")
  {
    brightnessPlugin = "winbrightness";
  }
  else if (kernel == "raspbian")
  {
    brightnessPlugin = "rpiblbrightness";
    brightnessSensor = "rpigpiobrightness";
  }
  else
  {
    brightnessPlugin = "xrandrbrightness";
  }
}

void CMainWindow::applyConfig ()
{
  SConfigTubes       tubes;
  SConfigThermometer thermometer;
  SConfigClock       clock;
  SConfigPositions   positions;
  SConfigWeather     weather;
  SConfigWebradios   webRadios;
  SConfigAlarms      alarms;
  SConfigTimers      timers;
  SConfigBrightness  brightness;

  pluginsFromKernel (brightness.m_plugin, brightness.m_sensor);

  QStringList iniFiles;
  iniFiles << QDir (applicationFolder ()).absoluteFilePath ("nixie.ini");
  iniFiles << QDir (appDataLocation ()).absoluteFilePath ("nixie.ini");
  for (QString const & iniFile : iniFiles)
  {
    if (QFile::exists (iniFile))
    {
      QSettings config (iniFile, QSettings::IniFormat);
      config.beginGroup ("tubes"); // [Tubes]
      keyValue (config, tubes.m_displayType, "display");
      keyValue (config, tubes.m_cClockTubes, "clockTubeNumber");
      keyValue (config, tubes.m_modeType, "mode");

      float       x, y, z;
      QTextStream reader;
      QString     value;
      keyValue (config, value, "backgroundUniformColor");
      if (!value.isEmpty ())
      {
        reader.setString (&value, QIODevice::ReadOnly);
        int r, g, b, a;
        reader >> r >> g >> b >> a;
        tubes.m_backgroundUniformColor = QColor (r, g, b, a);
      }

      if (keyValue (config, value, "moodLightPositions"))
      {
        reader.setString (&value, QIODevice::ReadOnly);
        reader >> x >> y >> z;
        tubes.m_moodLightPositions = QVector3D (x, y, z);
      }

      if (keyValue (config, value, "moodLightIntensities"))
      {
        reader.setString (&value, QIODevice::ReadOnly);
        reader >> x >> y >> z;
        tubes.m_moodLightIntensities = QVector3D (x, y, z);
      }

      keyValue (config, tubes.m_backgroundTexture, "backgroundTexture");
      keyValue (config, tubes.m_aboutTexture, "aboutTexture");
      keyValue (config, tubes.m_tableTexture, "tableTexture");
      keyValue (config, tubes.m_tableTexture, "tableTexture");
      keyValue (config, tubes.m_stlDir, "stl");
      keyValue (config, tubes.m_diaporamaDir, "diaporama");
      keyValue (config, tubes.m_diaporamaMode, "diaporamaMode");
      keyValue (config, tubes.m_diaporamaInterval, "diaporamaInterval");
      if (keyValue (config, value, "partCode"))
      {
        tubes.m_partCode = decodePartCode (value);
      }

      keyValue (config, tubes.m_forceBackgroungUniform, "forceBackgroungUniform");
      keyValue (config, tubes.m_fontFamily, "fontFamily");
      keyValue (config, tubes.m_menuFontCoef, "menuFontCoef");
      config.endGroup (); // [tubes]

      config.beginGroup ("thermometer"); // [Thermometer]
      keyValue (config, thermometer.m_celcius, "degree");

      keyValue (config, thermometer.m_sensorModel, "sensorModel");
      if (keyValue (config, value, "sensorId"))
      {
        thermometer.m_sensorId = value.toLatin1 ();
      }

      config.endGroup (); // [Thermometer]

      config.beginGroup ("clock"); // [Clock]
      keyValue (config, clock.m_h24, "h24");
      keyValue (config, clock.m_date, "date");
      keyValue (config, clock.m_format, "dateFormat");
      config.endGroup (); // [Clock]

      config.beginGroup ("positions"); // [positions]
      QStringList keys = config.childKeys ();
      for (QString const & key : keys)
      {
        bool ok;
        int  index = key.toInt (&ok);
        if (ok && index >= 0 && index <= 6)
        {
          QStringList locs = config.value (key, "(0 0 0)").toStringList ();
          float       x, y, z;
          char        c;
          int         i = 0;
          for (QString& loc : locs)
          {
            reader.setString (&loc, QIODevice::ReadOnly);
            while (!reader.atEnd ())
            {
              reader >> c >> x >> y >> z >> c;
              positions.m_ps[index][i++] = QVector3D (x, y, z);
            }
          }
        }
        else
        {
          if (key == "motion")
          {
            keyValue (config, positions.m_motion, "motion");
          }
          else if (key == "motionInterval")
          {
            keyValue (config, positions.m_motionInterval, "motionInterval");
          }
          else if (!key.startsWith ('#'))
          {
            QStringList values = config.value (key).toStringList ();
            if (values.size () == 3)
            {
              float v = values[0].toFloat (&ok);
              if (ok)
              {
                QVector3D coords;
                coords.setX (v);
                v = values[1].toFloat (&ok);
                if (ok)
                {
                  coords.setY (v);
                  v = values[2].toFloat (&ok);
                  if (ok)
                  {
                    coords.setZ (v);
                    if (key == "motionIncs")
                    {
                      positions.m_motionIncs = coords;
                    }
                    else if (key == "motionMin")
                    {
                      positions.m_motionMin = coords;
                    }
                    else if (key == "motionMax")
                    {
                      positions.m_motionMax = coords;
                    }
                  }
                }
              }
            }
          }
        }
      }

      config.endGroup (); // [positions]

      config.beginGroup ("weather"); // [weather]
      keyValue (config, weather.m_provider, "provider");
      keyValue (config, weather.m_town, "town");
      keyValue (config, weather.m_country, "country");
      keyValue (config, weather.m_locationUrl, "iploc");
      keyValue (config, weather.m_ipExternalUrl, "ipext");
      keyValue (config, weather.m_interval, "interval");
      keyValue (config, weather.m_activate, "active");
      config.endGroup (); // [weather]

      config.beginGroup ("webradios"); // [webradios]
      keys = config.childKeys ();
      for (QString const & key : keys)
      {
        if (key == "index")
        {
          keyValue (config, webRadios.m_index, "index");
        }
        else if (key == "volume")
        {
          keyValue (config, webRadios.m_volume, "volume");
        }
        else if (key == "mixer")
        {
#ifndef QTMULTIMEDIA_DEFINED
          keyValue (config, webRadios.m_mixer, "mixer");
#endif
        }
        else if (key == "player")
        {
#ifndef QTMULTIMEDIA_DEFINED
          keyValue (config, webRadios.m_player, "player");
#endif
        }
        else if (key == "mp3Only")
        {
          keyValue (config, webRadios.m_mp3, "mp3Only");
        }
        else if (key == "provider")
        {
          keyValue (config, webRadios.m_player, "provider");
        }
        else if (!key.startsWith ('#'))
        {
          QString::const_iterator it = key.cbegin (), end = key.cend ();
          for (; it != end && (*it).isDigit (); ++it);
          if (it == end)
          {
            CStation s;
            decodeStation (s, config.value (key).toString ());
            webRadios.m_stations.push_back (s);
          }
        }
      }
      config.endGroup (); // [webradios]

      config.beginGroup ("alarms"); // [alarms]
      keys = config.childKeys ();
      for (QString const & key : keys)
      {
        if (!key.startsWith ('#'))
        {
          bool ok;
          key.toInt (&ok);
          if (ok)
          {
            QString     line = config.value (key).toString ();
            QStringList data = line.split ('|');
            if (data.size () >= 3)
            {
              unsigned  days = data[2].toUInt ();
              QDateTime dt;
              dt.setMSecsSinceEpoch (data[1].toLongLong ());
              QDateTime currentDateTime = QDateTime::currentDateTime ();
              if (dt <= currentDateTime && days != 0)
              {
                dt = dt.addDays (1);
              }

              if (dt > currentDateTime)
              {
                CAlarm* alarm = new CAlarm;
                alarm->setDateTime (dt);
                alarm->setName (data[0]);
                alarm->setDays (data[2].toUInt ());
                alarms.m_alarms.push_back (alarm);
              }
            }
          }
          else if (key == "repeat")
          {
            keyValue (config, alarms.m_repeat, "repeat");
          }
          else if (key == "interval")
          {
            keyValue (config, alarms.m_interval, "interval");
          }
          else if (key == "volumeMax")
          {
            keyValue (config, alarms.m_volumeMax, "volumeMax");
          }
          else if (key == "volumeMin")
          {
            keyValue (config, alarms.m_volumeMin, "volumeMin");
          }
          else if (key == "volumeStep")
          {
            keyValue (config, alarms.m_volumeStep, "volumeStep");
          }
          else if (key == "ringtone")
          {
            if (keyValue (config, value, "ringtone"))
            {
              alarms.m_ringtone = value;
            }
          }
        }
      }
      config.endGroup (); // [alarms]

      config.beginGroup ("timers"); // [timers]
      keys = config.childKeys ();
      for (QString const & key : keys)
      {
        if (!key.startsWith ('#'))
        {
          if (key == "ringtone")
          {
            keyValue (config, value, "ringtone");
            if (!value.isEmpty ())
            {
              timers.m_ringtone = value;
            }
          }
          else
          {
            QString     values = config.value (key).toString ();
            QStringList elems  = values.split ('|');
            if (elems.size () == 2)
            {
              CTimer* timer = new CTimer;
              timer->setName (elems[0]);
              timer->setRemainingTime (elems[1].toUInt ());
              timer->setEnabled (false);
              m_timers.push_back (timer);
            }
          }
        }
      }

      config.endGroup (); // [timers]

      config.beginGroup ("brightness"); // [brightness]
      keyValue (config, brightness.m_gpiopin, "gpiopin");
      keyValue (config, brightness.m_active, "active");
      keyValue (config, brightness.m_hight, "hight");
      keyValue (config, brightness.m_low, "low");
      keyValue (config, brightness.m_plugin, "brightness");
      keyValue (config, brightness.m_sensor, "sensor");
      config.endGroup (); // [brightness]
    }
  }

  ui->m_device->setDisplay (tubes.m_displayType);
  m_cClockTubes = tubes.m_cClockTubes;
  ui->m_device->setBackgroundUniformColor (tubes.m_backgroundUniformColor);
  ui->m_device->setMoodLightPositions (tubes.m_moodLightPositions.x (), tubes.m_moodLightPositions.y (), tubes.m_moodLightPositions.z ());
  ui->m_device->setMoodLightIntensities (tubes.m_moodLightIntensities.x (), tubes.m_moodLightIntensities.y (), tubes.m_moodLightIntensities.z ());
  ui->m_device->setBackgroundTexture (defaultFolder (tubes.m_backgroundTexture));
  ui->m_device->setAboutTexture (defaultFolder (tubes.m_aboutTexture));
  ui->m_device->setTableTexture (defaultFolder (tubes.m_tableTexture));
  ui->m_device->setStlDirName (defaultFolder (tubes.m_stlDir));
  if (!tubes.m_diaporamaDir.isEmpty ())
  {
    QDir        dir (defaultFolder (tubes.m_diaporamaDir));
    QStringList filters;
    filters << "*.jpeg" << "*.jpg" << "*.png" << "*.tif" << "*.tiff" << "*.bmp" << "*.gif";
    QStringList fileNames = dir.entryList (filters , QDir::Files, QDir::Name);
    for (QString& fileName : fileNames)
    {
      fileName = dir.absoluteFilePath (fileName);
    }

    ui->m_device->setDiaporamaImages (fileNames);
  }

  ui->m_device->setDiaporamaMode (tubes.m_diaporamaMode);
  ui->m_device->setDiaporamaInterval (tubes.m_diaporamaInterval);
  ui->m_device->setPartCode (tubes.m_partCode);
  if (tubes.m_forceBackgroungUniform)
  {
    forceReleaseBackgroungUniform ();
  }

  if (!tubes.m_fontFamily.isEmpty ())
  {
    ui->m_device->setFontFamily (tubes.m_fontFamily);
    setFontFamily (tubes.m_fontFamily);
  }

  if (!brightness.m_plugin.isEmpty ())
  {
    QByteArray pluginVersion;
    QObject*   plugin = ::loadPlugin (brightness.m_plugin, &pluginVersion);
    if (plugin != nullptr)
    {
      m_brightnessPlugin = qobject_cast<CBrightnessInterface*>(plugin);
      if (m_brightnessPlugin != nullptr &&
          (!m_brightnessPlugin->isCompatible (pluginVersion) ||
           !m_brightnessPlugin->connected ()))
      {
        ::unloadPlugin (brightness.m_plugin);
        m_brightnessPlugin = nullptr;
      }
    }
  }

  // For touch device increase menu font by 1.6 if tubes.m_menuFontCoef = 0.
  if (tubes.m_menuFontCoef == 0.0f)
  {
    tubes.m_menuFontCoef = !touchScreenActivated () ? 1.0f : 1.6f;
  }

  m_menuFontCoef = tubes.m_menuFontCoef;
  m_celcius      = thermometer.m_celcius;
  m_h24          = clock.m_h24;
  ui->m_date->setHidden (!clock.m_date);
  CAlarm::setDateFormat (clock.m_format);

  for (unsigned index : { 3, 4, 6 })
  {
    for (unsigned j = 0; j < index; ++j)
    {
      ui->m_device->addPos (index, positions.m_ps[index][j]);
    }
  }

  ui->m_device->setInMotion (positions.m_motion);
  ui->m_device->setMotionInterval (positions.m_motionInterval);
  ui->m_device->setMotionIncr (positions.m_motionIncs);
  ui->m_device->setMotionMin (positions.m_motionMin);
  ui->m_device->setMotionMax (positions.m_motionMax);

  if (!weather.m_provider.isEmpty ())
  {
    QByteArray pluginVersion;
    QObject*   plugin = ::loadPlugin (weather.m_provider, &pluginVersion);
    if (plugin != nullptr)
    {
      m_weather = qobject_cast<CWeatherInterface*>(plugin);
      if (m_weather != nullptr && m_weather->isCompatible (pluginVersion))
      {
        m_weatherInterval = weather.m_interval * 60 * 1000;
        m_weather->setTown (weather.m_town);
        m_weather->setCountry (weather.m_country);
        m_locationUrl   = weather.m_locationUrl;
        m_ipExternalUrl = weather.m_ipExternalUrl;
        if (weather.m_activate)
        {
          startWeather ();
        }
      }
      else
      {
        ::unloadPlugin (weather.m_provider);
        m_weather = nullptr;
      }
    }
  }

  weatherSetHidden (m_weather == nullptr || !weather.m_activate);
#ifndef QTMULTIMEDIA_DEFINED
  m_webRadios.setMixerProgram (webRadios.m_mixer);
  m_webRadios.setPlayerProgram (webRadios.m_player);
#endif
  if (!webRadios.m_provider.isEmpty ())
  {
    QByteArray pluginVersion;
    QObject*   plugin = ::loadPlugin (webRadios.m_provider, &pluginVersion);
    if (plugin != nullptr)
    {
      m_webRadiosProvider = qobject_cast<CWebRadiosInterface*>(plugin);
      if (!m_webRadiosProvider->isCompatible (pluginVersion))
      {
        ::unloadPlugin (webRadios.m_provider);
        m_webRadiosProvider = nullptr;
      }
      else
      {
        m_webRadiosProvider->setMp3Only (webRadios.m_mp3);
      }
    }
  }

  for (CStation const & station : webRadios.m_stations)
  {
    m_webRadios.add (station);
  }

  m_webRadios.setCurrentIndex (webRadios.m_index);
  on_m_volume_valueChanged (webRadios.m_volume);

  if (!alarms.m_alarms.isEmpty ())
  {
    m_alarms.reserve (alarms.m_alarms.size ());
    for (CAlarm* alarm : alarms.m_alarms)
    {
      m_alarms.push_back (alarm);
    }
  }

  CAlarm::setRepeatInterval (alarms.m_interval * 60 * 1000);
  CAlarm::setRepeatCount (alarms.m_repeat);
  m_ringtoneVolumeMax   = alarms.m_volumeMax;
  m_ringtoneVolumeMin   = alarms.m_volumeMin;
  m_ringtoneVolumeStep  = alarms.m_volumeStep;
  m_alarmRingtoneURL    = ringtoneUrl (alarms.m_ringtone);
  m_timerRingtoneURL    = ringtoneUrl (timers.m_ringtone);

  if (!brightness.m_sensor.isEmpty ())
  {
    QByteArray pluginVersion;
    QObject*   plugin = ::loadPlugin (brightness.m_sensor, &pluginVersion);
    if (plugin != nullptr)
    {
      m_brightnessSensorPlugin = qobject_cast<CBrightnessSensorInterface*>(plugin);
      if (!m_brightnessSensorPlugin->isCompatible (pluginVersion))
      {
        ::unloadPlugin (brightness.m_sensor);
        m_brightnessSensorPlugin = nullptr;
      }
      else
      {
        m_brightnessSensorPlugin->setId (QByteArray::number (brightness.m_gpiopin));
      }
    }
  }

  if (m_brightnessSensorPlugin != nullptr)
  {
    m_brightness      = new CBrightness (m_brightnessSensorPlugin, brightness.m_active, this);
    m_hightBrightness = brightness.m_hight;
    m_lowBrightness   = brightness.m_low;
    if (m_hightBrightness < m_lowBrightness)
    {
      std::swap (m_hightBrightness, m_lowBrightness);
    }

    m_brightness->start ();
  }

  installTranslator ();

  // Create temperature sensor.
  if (thermometer.m_sensorModel == "ds18b20" && thermometer.m_sensorId.isEmpty ())
  {
    thermometer.m_sensorId = ds18b20Detection ();
  }

  if (!thermometer.m_sensorModel.isEmpty () && !thermometer.m_sensorId.isEmpty ())
  {
    QByteArray  metadataVersion;
    QObject*    plugin = ::loadPlugin (thermometer.m_sensorModel, &metadataVersion);
    if (plugin != nullptr)
    {
      m_ambiantTemperatureSensor = qobject_cast<CTempSensorInterface*>(plugin);
      bool validPlugin           = false;
      if (m_ambiantTemperatureSensor != nullptr && m_ambiantTemperatureSensor->isCompatible (metadataVersion))
      {
        m_ambiantTemperatureSensor->setId (thermometer.m_sensorId);
        validPlugin = m_ambiantTemperatureSensor->value () != VTOOLS_INVALID_TEMPERATURE;
      }

      if (!validPlugin)
      {
        ::unloadPlugin (thermometer.m_sensorModel);
        m_ambiantTemperatureSensor = nullptr;
      }
    }
  }

  if (tubes.m_modeType == CMode::Thermometer)
  { // The last mode was thermometer
    ui->m_device->setNixieTubeNumber (3);
    this->thermometer ();
  }
  else
  { // The last mode was other modes, force clock mode.
    ui->m_device->setNixieTubeNumber (m_cClockTubes);
    this->clock ();
  }

  if (!m_alarms.isEmpty ())
  {
    launchAlarms ();
  }
}

void CMainWindow::openGLConfig (SOpenGlParams& params)
{
  if (QSysInfo::productType () == "raspbian" && touchScreenActivated ())
  {
     params.m_showTitle     = false;
     params.m_maxWindowSize = true;
  }

  QString   fileName = QDir (applicationFolder ()).absoluteFilePath ("nixie.ini");
  QSettings config (fileName, QSettings::IniFormat);

  QString value;
  config.beginGroup ("opengl");
  keyValue (config, params.m_samples, "samples");
  keyValue (config, params.m_depthBufferSize, "depthBufferSize");
  keyValue (config, params.m_showTitle, "title");
  if (keyValue (config, value, "size"))
  {
    params.m_maxWindowSize = value.toLower () == "max";
  }

  if (keyValue (config, value, "geometry"))
  {
    QTextStream reader (&value, QIODevice::ReadOnly);
    int         l, t, w, h, s;
    reader >> l >> t >> w >> h >> s;
    params.m_rc = QRect (l, t, w, h);
    params.m_ws = static_cast<Qt::WindowStates>(s);
  }

  config.endGroup ();
}

void CMainWindow::saveConfig ()
{
  QDir dir = appDataLocation ();
  if (!dir.exists ())
  {
    dir.cdUp ();
    dir.mkdir ("nixie");
    dir.cd ("nixie");
  }

  QString   fileName = dir.absoluteFilePath ("nixie.ini");
  QSettings config (fileName, QSettings::IniFormat);

  config.beginGroup ("tubes");
  config.setValue ("display", ui->m_device->display ());
  config.setValue ("fontFamily", ui->m_device->fontFamily ());

  CMode::EType type = ui->m_device->mode ()->type ();
  if (type == CMode::Clock || type == CMode::Thermometer)
  {
    config.setValue ("mode", type);
  }

  config.setValue ("clockTubeNumber", m_cClockTubes);
  QString partCode ('*');
  if (!ui->m_device->isVisiblePart (CNixie::PCTable))
  {
    partCode = "-table";
  }

  if (!ui->m_device->isVisiblePart (CNixie::PCPencil))
  {
    if (partCode[0] == '*')
    {
      partCode.clear ();
    }
    else
    {
      partCode += ' ';
    }

    partCode += "-pencil";
  }

  config.setValue ("partCode", partCode);
  config.setValue ("forceBackgroungUniform", ui->m_device->isBackgroungUniform ());
  config.endGroup ();

  config.beginGroup ("clock");
  config.setValue ("h24", m_h24);
  config.setValue ("date", !ui->m_date->isHidden ());
  config.endGroup ();

  config.beginGroup ("weather");
  config.setValue ("town", m_weather->town ());
  config.setValue ("country", m_weather->country ());
  config.setValue ("active", !ui->m_w9->isHidden ());
  config.endGroup ();

  config.beginGroup ("opengl");
  QRect            rc = normalGeometry ();
  Qt::WindowStates ws = windowState ();
  QString          st = QString ("%1 %2 %3 %4 %5").arg (rc.left ()).arg (rc.top ())
                           .arg (rc.width ()).arg (rc.height ()).arg (ws);
  config.setValue ("geometry", st);
  config.endGroup ();

  config.beginGroup ("webradios");
  removeNumericalSettingKeys (config);
  config.setValue ("index", m_webRadios.currentIndex ());
  config.setValue ("volume", m_webRadios.volume ());
  QVector<CStation> const & stations = m_webRadios.stations ();
  int index = 1;
  for (CStation const & station : stations)
  {
    QString setting = station.name () + '|' + station.logo ().toString () + '|' + station.url ().toString ();
    config.setValue (QString::number (index), setting);
    ++index;
  }
  config.endGroup ();

  config.beginGroup ("positions");
  config.setValue ("motion", ui->m_device->inMotion ());
  config.endGroup ();

  config.beginGroup ("thermometer");
  config.setValue ("degree", m_celcius);
  config.endGroup ();

  config.beginGroup ("alarms");
  removeNumericalSettingKeys (config);
  for (int iAlarm = 0, cAlarms = m_alarms.size (); iAlarm < cAlarms; ++iAlarm)
  {
    CAlarm const * alarm = m_alarms[iAlarm];
    QString        key   = QString ("%1").arg (iAlarm);
    QString        value = QString ("%1|%2|%3").arg (alarm->name ())
                                               .arg (alarm->dateTime().toMSecsSinceEpoch())
                                               .arg (alarm->days ());
    config.setValue (key, value);
  }
  config.endGroup ();

  config.beginGroup ("timers");
  removeNumericalSettingKeys (config);
  for (int iTimer = 0, cTimers = m_timers.size (); iTimer < cTimers; ++iTimer)
  {
    QString key   = QString ("%1").arg (iTimer);
    QString value = QString ("%1|%2").arg (m_timers[iTimer]->name ()).arg (m_timers[iTimer]->initialTime ());
    config.setValue (key, value);
  }

  config.endGroup ();
}

void CMainWindow::restoreGeometry (QRect const & rc, Qt::WindowStates ws)
{
  if (rc.isValid ())
  {
    QRect wRect = rc;
    QRect sRect = QApplication::desktop ()->screenGeometry ();
    int   dx   = 0, dy = 0;
    if (wRect.right () > sRect.right ())
    {
      dx = sRect.right () - wRect.right ();
    }
    else if (wRect.left () < sRect.left ())
    {
      dx = wRect.left () - sRect.left ();
    }
    else if (wRect.bottom () > sRect.bottom ())
    {
      dy = sRect.bottom () - sRect.bottom ();
    }
    else if (wRect.top () < sRect.top ())
    {
      dy = wRect.top () - sRect.top ();
    }

    if (dx != 0 || dy != 0)
    {
      wRect.translate (dx, dy);
    }

    setGeometry (wRect);
    setWindowState (ws);
  }
}



