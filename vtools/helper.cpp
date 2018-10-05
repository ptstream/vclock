#include "helper.hpp"
#include "nam.hpp"
#include <QCoreApplication>
#include <QPluginLoader>
#include <QFileInfo>
#include <QDir>
#include <QLocale>
#include <QTranslator>
#include <QUrl>
#include <QSettings>
#include <QStandardPaths>
#include <QTouchDevice>

QString VTOOLSSHARED_EXPORT applicationFolder ()
{
  QFileInfo fileInfo (QCoreApplication::applicationFilePath ());
  return fileInfo.absolutePath ();
}

QString VTOOLSSHARED_EXPORT appDataLocation ()
{
  return QStandardPaths::writableLocation (QStandardPaths::AppDataLocation);
}

QString VTOOLSSHARED_EXPORT defaultFolder (QString name)
{
  QFileInfo fi (name);
  QString   path = fi.path ();
  if (path.startsWith ("$exe$"))
  {
    name.remove ("$exe$");
    if (name.startsWith ('/'))
    {
      name.remove (0, 1);
    }

    name = QDir (applicationFolder ()).absoluteFilePath (name);
  }

  return name;
}

void VTOOLSSHARED_EXPORT installTranslator (QString locale)
{
  if (locale.isEmpty ())
  {
    locale = QLocale::system ().name ().section ('_', 0, 0);
  }

  QDir        dir (applicationFolder () + "/languages");
  QStringList filters ("*_" + locale + ".qm");
  QStringList files = dir.entryList (filters, QDir::Files | QDir::NoDotAndDotDot);
  if (!files.isEmpty ())
  {
    for (QString const & file : files)
    {
      QString      path       = dir.absoluteFilePath (file);
      QTranslator* translator = new QTranslator;
      translator->load (path);
      QCoreApplication::installTranslator (translator);
    }
  }
}

QString VTOOLSSHARED_EXPORT decodeM3u (QByteArray m3u)
{
  QString url;
  m3u.replace ('\r', QString ());
  QList<QByteArray> lines = m3u.split ('\n');
  for (QByteArray const & line : lines)
  {
    if (line.startsWith ("http"))
    {
      url = QString::fromUtf8 (line);
      break;
    }
  }

  return url;
}

void VTOOLSSHARED_EXPORT removeNumericalSettingKeys (QSettings& settings)
{
  QStringList keys = settings.childKeys ();
  for (QString const & key : keys)
  {
    int k, end = key.length ();
    for (k = 0; k < end && key[k].isNumber (); ++k);
    if (k == end)
    {
      settings.remove (key);
    }
  }
}

QString VTOOLSSHARED_EXPORT formatItem (int i)
{
  return QString ("%1").arg (i, 2, 10, QLatin1Char ('0'));
}

QObject* VTOOLSSHARED_EXPORT loadPlugin (QString const & name, QByteArray* version)
{
  QDir pluginsDir = QDir (qApp->applicationDirPath ());
  pluginsDir.cd ("plugins");
  QPluginLoader loader (pluginsDir.absoluteFilePath (name));
  QObject*      instance = loader.instance ();
  if (version != nullptr)
  {
    *version = loader.metaData ().value ("MetaData").toObject ().value ("InterfaceVersion").toString ().toLatin1 ();
  }

  return instance;
}

void VTOOLSSHARED_EXPORT unloadPlugin (QString const & name)
{
  QDir pluginsDir = QDir (qApp->applicationDirPath ());
  pluginsDir.cd ("plugins");
  QPluginLoader loader (pluginsDir.absoluteFilePath (name));
  loader.unload ();
}

QPixmap VTOOLSSHARED_EXPORT logo (QUrl const & url, QNetworkAccessManager* nam)
{
  QPixmap pxm;
  if (!url.isEmpty ())
  {
    QByteArray data = CNam ().callData (url, nam);
    if (!data.isEmpty ())
    {
      pxm.loadFromData (data);
    }
  }
  else
  {
    pxm = QPixmap (":/icons/radio.png");
  }

  return pxm;
}

bool VTOOLSSHARED_EXPORT touchScreenActivated ()
{
  static int touchScreen = -1;
  if (touchScreen == -1)
  {
    touchScreen                         = 0;
    QList<QTouchDevice const *> devices = QTouchDevice::devices ();
    if (!devices.isEmpty ())
    {
      for (QTouchDevice const * device : devices)
      {
        if (device->type () == QTouchDevice::TouchScreen)
        {
          touchScreen = 1;
          break;
        }
      }
    }
  }

  return touchScreen == 1;
}

QByteArray VTOOLSSHARED_EXPORT ds18b20Detection ()
{
  QByteArray    id;
  QDir          dir ("/sys/bus/w1/devices/w1_bus_master1");
  QStringList   filters ("28-*");
  QFileInfoList w1Devices = dir.entryInfoList (filters, QDir::Dirs | QDir::NoDotAndDotDot);
  if (!w1Devices.isEmpty ())
  {
    id = w1Devices.first ().baseName ().toLatin1 ();
  }

  return id;
}
