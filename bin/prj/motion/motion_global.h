///-----------------------------------------------------------------------------
#ifndef MOTION_GLOBAL_H
#define MOTION_GLOBAL_H
///-----------------------------------------------------------------------------
#include <QtCore/qglobal.h>
///-----------------------------------------------------------------------------
#if defined(MOTION_LIBRARY)
    #define MOTIONSHARED_EXPORT Q_DECL_EXPORT
#else
    #define MOTIONSHARED_EXPORT Q_DECL_IMPORT
#endif
///-----------------------------------------------------------------------------
#endif // MOTION_GLOBAL_H
///-----------------------------------------------------------------------------
