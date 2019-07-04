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
#include "../direct/direct.h"
#include "../../../src/dsp/PC-DSP.h"
#include "../../../src/motion/common.h"

///-----------------------------------------------------------------------------
typedef int CONSOLE_HANDLER(const char *buf);
typedef void ERRMSG_HANDLER(const char *ErrMsg);
///-----------------------------------------------------------------------------
class MOTIONSHARED_EXPORT MotionClass
{
    public:
        MotionClass(int boardId);
        virtual ~MotionClass();
        int     _boardID;

        int     WriteLineReadLine(const char *s,char *response);
        int     WriteLine(const char *s);
        int     WriteLineWithEcho(const char *s);
        int     ReadLineTimeOut(char *buf,int TimeOutms = 1000000);
        int     ListLocations(int *nlocations,int *list);
        int     waitToken(bool display_msg = true,int TimeOut_ms = 1000000,char* CallerID = nullptr);
        int     motionLock(char *CallerID = nullptr);
        int     usbLocation();
        int     motionLockRecovery();
        void    releaseToken();
        int     failed();
        int     disconnect();
        int     firmwareVersion();
        int     checkForReady();

        int     ServiceConsole();
        int     SetConsoleCallback(CONSOLE_HANDLER *ch);
        int     SetErrMsgCallback(ERRMSG_HANDLER *eh);
        int     CheckKMotionVersion(int *type = nullptr,bool GetBoardTypeOnly = false);

        int     GetStatus(MAIN_STATUS& status,bool lock);
        void    DoErrMsg(const char *s);

        bool    _errMessageDisplayed;
    private:
        QMutex* _pipeMutex;
        bool    _pipeOpen;
        bool    _serverMessDisplayed;

        CONSOLE_HANDLER *consoleHandler;
        ERRMSG_HANDLER *errMsgHandler;

        int pipeCmd(int code);
        int pipeCmdStr(int code,const char *s);
        int pipe(const char *s,int n,char *r,int *m);
        int launchServer();

        QString ExtractPath(QString InFile);
        QFile   PipeFile;

};
///-----------------------------------------------------------------------------
#endif /// MOTION_H
///-----------------------------------------------------------------------------


