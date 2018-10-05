#ifndef HELPER_HPP
#define HELPER_HPP

#include "vtools_globals.h"
#include <QPixmap>

class QSettings;
class QNetworkAccessManager;

#ifndef Q_OS_WIN
#undef VTOOLSSHARED_EXPORT
#define VTOOLSSHARED_EXPORT
#endif

/*! Returns the application folder. In fact the folder of the excecutable file. */
QString applicationFolder ();

/*! Returns the application data folder. */
QString VTOOLSSHARED_EXPORT appDataLocation ();

/*! Returns the application folder if name contains '$exe$'. */
QString VTOOLSSHARED_EXPORT defaultFolder (QString name);

/*! Loads all qm files of the languages folder. */
void VTOOLSSHARED_EXPORT installTranslator (QString locale = QString ());

/*! Removes numerical keys of a ini file section. */
void VTOOLSSHARED_EXPORT removeNumericalSettingKeys (QSettings& settings);

/*! Returns an integer value [0, 99] in form 01, 02, 03, ... 10, ..*/
QString VTOOLSSHARED_EXPORT formatItem (int i);

/*! Loads an returns a plugin after version verification. */
QObject* VTOOLSSHARED_EXPORT loadPlugin (QString const & name, QByteArray* version = nullptr);

/*! Unloads a plugin. */
void VTOOLSSHARED_EXPORT unloadPlugin (QString const & name);

/*! returns the QPixmap from an url.
 * \param url: The pixmap url.
 * \param nam: A global QNetworkAccessManager.
 */
QPixmap VTOOLSSHARED_EXPORT logo (QUrl const & url, QNetworkAccessManager* nam);

/*! Returns true if computer has a touch screen. */
bool VTOOLSSHARED_EXPORT touchScreenActivated ();

/*! Returns identifier of the first DS18B20 detected on the Raspberry. */
QByteArray VTOOLSSHARED_EXPORT ds18b20Detection ();

#endif // HELPER_HPP
