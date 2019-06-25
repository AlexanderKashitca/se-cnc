///-----------------------------------------------------------------------------
#ifndef MOTION_H
#define MOTION_H
///-----------------------------------------------------------------------------
#include <QFile>
#include <QMutex>
#include <QString>
///-----------------------------------------------------------------------------
#include "motion_global.h"
///-----------------------------------------------------------------------------
#include "../../../src/motion/common.h"
#include "../../../src/direct/motion_direct.h"
///-----------------------------------------------------------------------------
typedef int CONSOLE_HANDLER(const char *buf);
typedef void ERRMSG_HANDLER(const char *ErrMsg);
///-----------------------------------------------------------------------------
class MOTIONSHARED_EXPORT MotionClass
{
    public:
        MotionClass(int boardid);
        virtual ~MotionClass();
        int BoardID;

        int     WriteLineReadLine(const char *s,char *response);
        int     WriteLine(const char *s);
        int     WriteLineWithEcho(const char *s);
        int     ReadLineTimeOut(char *buf,int TimeOutms = 1000000);
        int     ListLocations(int *nlocations,int *list);
        int     WaitToken(bool display_msg = true,int TimeOut_ms = 1000000,char* CallerID = nullptr);
        int     MotionLock(char *CallerID = nullptr);
        int     USBLocation();
        int     MotionLockRecovery();
        void    ReleaseToken();
        int     Failed();
        int     Disconnect();
        int     FirmwareVersion();
        int     CheckForReady();

        int     ServiceConsole();
        int     SetConsoleCallback(CONSOLE_HANDLER *ch);
        int     SetErrMsgCallback(ERRMSG_HANDLER *eh);
        int     CheckKMotionVersion(int *type = nullptr,bool GetBoardTypeOnly = false);

        int     GetStatus(MAIN_STATUS& status,bool lock);
        void    DoErrMsg(const char *s);

        bool    ErrMessageDisplayed;
    private:
        QMutex *PipeMutex;
        bool    PipeOpen;
        bool    ServerMessDisplayed;

        CONSOLE_HANDLER *ConsoleHandler;
        ERRMSG_HANDLER *ErrMsgHandler;

        int PipeCmd(int code);
        int PipeCmdStr(int code,const char *s);
        int Pipe(const char *s,int n,char *r,int *m);
        int LaunchServer();

        QString ExtractPath(QString InFile);
        QFile PipeFile;

};
///-----------------------------------------------------------------------------
#endif /// MOTION_H
///-----------------------------------------------------------------------------


