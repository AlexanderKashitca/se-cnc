///-----------------------------------------------------------------------------
#ifndef GCODEINTERPRETER_GLOBAL_H
#define GCODEINTERPRETER_GLOBAL_H
///-----------------------------------------------------------------------------
#include <QtCore/qglobal.h>
///-----------------------------------------------------------------------------
#if defined(GCODEINTERPRETER_LIBRARY)
    #define GCODEINTERPRETER_API Q_DECL_EXPORT
#else
    #define GCODEINTERPRETER_API Q_DECL_IMPORT
#endif
///-----------------------------------------------------------------------------
#endif /// GCODEINTERPRETER_GLOBAL_H
///-----------------------------------------------------------------------------
