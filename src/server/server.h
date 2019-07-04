///-----------------------------------------------------------------------------
#ifndef SERVER_H
#define SERVER_H
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
#endif /// SERVER_H
///-----------------------------------------------------------------------------
