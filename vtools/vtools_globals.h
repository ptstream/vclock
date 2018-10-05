#ifndef VTOOLS_GLOBALS_H
#define VTOOLS_GLOBALS_H

#include <QtCore/qglobal.h>

#ifdef Q_OS_WIN
#if defined(VTOOLS_LIBRARY)
#  define VTOOLSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define VTOOLSSHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#  define VTOOLSSHARED_EXPORT
#endif

#define VTOOLS_INVALID_TEMPERATURE -9999.9f

template <typename T>
inline T toCelcius (T t)
{
  return (t - 32.0) * 5.0 / 9.0;
}

template <typename T>
inline T toFahrenheit (T t)
{
  return 9.0 * t / 5.0 + 32.0;
}

#endif // VTOOLS_GLOBALS_H
