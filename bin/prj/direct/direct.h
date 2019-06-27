///-----------------------------------------------------------------------------
#ifndef DIRECT_H
#define DIRECT_H
///-----------------------------------------------------------------------------
#include "direct_global.h"
///-----------------------------------------------------------------------------
#include "../../../src/motion/motion_io.h"
///#include <stddef.h>
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

    int     MapBoardToIndex(int BoardID);
    int     WriteLineReadLine(const char *s,char *response);
    int     WriteLine(const char *s);
    int     WriteLineWithEcho(const char *s);
    int     ReadLineTimeOut(char *buf,int TimeOutms);
    int     ListLocations(int *nlocations,int *list);
    void    WaitToken(char *CallerID = nullptr);
    int     MotionLock(char *CallerID = nullptr);
    int     USBLocation();
    int     MotionLockRecovery();
    void    ReleaseToken();
    int     Failed();
    int     Disconnect();
    int     FirmwareVersion();
    int     CheckForReady();
    int     ServiceConsole();
    const char *GetErrMsg();
    void    ClearErrMsg();
    int     SetConsoleCallback(SERVER_CONSOLE_HANDLER *ch);
    int     nInstances();
};
///-----------------------------------------------------------------------------
#endif /// DIRECT_H
///-----------------------------------------------------------------------------
