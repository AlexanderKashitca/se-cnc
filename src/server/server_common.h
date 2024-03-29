///-----------------------------------------------------------------------------
#ifndef SERVER_COMMON_H
#define SERVER_COMMON_H
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
    ENUM_SetConsole,
    ENUM_Echo
};
///-----------------------------------------------------------------------------
enum
{
    DEST_NORMAL,
    DEST_CONSOLE,
    DEST_ERRMSG
};
///-----------------------------------------------------------------------------
#endif /// SERVER_COMMON_H
///-----------------------------------------------------------------------------
