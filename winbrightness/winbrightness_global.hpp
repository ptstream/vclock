#ifndef WINBRIGHTNESS_GLOBAL_HPP
#define WINBRIGHTNESS_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(WINBRIGHTNESS_LIBRARY)
#  define WINBRIGHTNESSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define WINBRIGHTNESSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RPIBLBRIGHTNESS_GLOBAL_HPP
