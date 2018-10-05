#ifndef API_IPIFY_ORG_GLOBAL_HPP
#define API_IPIFY_ORG_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(API_IPIFY_ORG_LIBRARY)
#  define API_IPIFY_ORGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define API_IPIFY_ORGSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // API_IPIFY_ORG_GLOBAL_HPP