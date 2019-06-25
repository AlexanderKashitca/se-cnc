#ifndef DIRECT_GLOBAL_H
#define DIRECT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DIRECT_LIBRARY)
#  define DIRECTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DIRECTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DIRECT_GLOBAL_H
