#ifndef IP_API_COM_GLOBAL_HPP
#define IP_API_COM_GLOBAL_HPP

#include <QtCore/qglobal.h>

#if defined(IP_API_COM_LIBRARY)
#  define IP_API_COMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define IP_API_COMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IP_API_COM_GLOBAL_HPP
