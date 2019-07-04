///-----------------------------------------------------------------------------
#ifndef DIRECT_H
#define DIRECT_H
///-----------------------------------------------------------------------------
#include <QLibrary>
#include "direct_global.h"
#include "../../../src/motion/motion_io.h"
///-----------------------------------------------------------------------------
class DIRECTSHARED_EXPORT MotionDirectClass
{
    private:
        MotionIOClass MotionIO;
        QLibrary library;
        bool    _ftdiLibraryLoad;
    public:
                MotionDirectClass();
        bool    getFtdiLibraryLoad();
        int     mapBoardToIndex(int BoardID);
        int     writeLineReadLine(const char *s,char *response);
        int     writeLine(const char *s);
        int     writeLineWithEcho(const char *s);
        int     readLineTimeOut(char *buf,int TimeOutms);
        int     listLocations(int *nlocations,int *list);
        void    waitToken(char *CallerID = nullptr);
        int     motionLock(char *CallerID = nullptr);
        QString usbLocation();
        int     motionLockRecovery();
        void    releaseToken();
        int     failed();
        int     disconnect();
        int     firmwareVersion();
        int     checkForReady();
        int     serviceConsole();
        const char* getErrMsg();
        void    clearErrMsg();
        int     setConsoleCallback(SERVER_CONSOLE_HANDLER *ch);
        int     nInstances();
};
///-----------------------------------------------------------------------------
#endif /// DIRECT_H
///-----------------------------------------------------------------------------
