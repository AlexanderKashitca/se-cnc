///-----------------------------------------------------------------------------
#ifndef DIRECT_H
#define DIRECT_H
///-----------------------------------------------------------------------------
#include "direct_global.h"
#include "../../../src/motion/motion_io.h"
///-----------------------------------------------------------------------------
enum
{
    ENUM_WriteLineReadLine,
    ENUM_WriteLine,
    ENUM_WriteLineWithEcho,
    ENUM_ReadLineTimeOut,
    ENUM_ListLocations,
    ENUM_Failed,
    ENUM_Disconnect,
    ENUM_FirmwareVersion,
    ENUM_CheckForReady,
    ENUM_KMotionLock,
    ENUM_USBLocation,
    ENUM_KMotionLockRecovery,
    ENUM_ReleaseToken,
    ENUM_ServiceConsole,
    ENUM_SetConsole
};
///-----------------------------------------------------------------------------
enum
{
    DEST_NORMAL,
    DEST_CONSOLE,
    DEST_ERRMSG
};
///-----------------------------------------------------------------------------
class DIRECTSHARED_EXPORT MotionDirectClass
{
    private:
        MotionIOClass MotionIO;
    public:
        MotionDirectClass(void);

        int     mapBoardToIndex(int BoardID);
        int     writeLineReadLine(const char *s,char *response);
        int     writeLine(const char *s);
        int     writeLineWithEcho(const char *s);
        int     readLineTimeOut(char *buf,int TimeOutms);
        int     listLocations(int *nlocations,int *list);
        void    waitToken(char *CallerID = nullptr);
        int     motionLock(char *CallerID = nullptr);
        int     usbLocation();
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
