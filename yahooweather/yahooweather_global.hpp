#ifndef YAHOOWEATHER_GLOBAL_HPP
#define YAHOOWEATHER_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(YAHOOWEATHER_LIBRARY)
#  define YAHOOWEATHERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define YAHOOWEATHERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // YAHOOWEATHER_GLOBAL_HPP