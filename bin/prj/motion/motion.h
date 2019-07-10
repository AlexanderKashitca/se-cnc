///-----------------------------------------------------------------------------
#ifndef MOTION_H
#define MOTION_H
///-----------------------------------------------------------------------------
#include <QString>
#include <QtDBus/QtDBus>
#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtDBus/QDBusInterface>
///-----------------------------------------------------------------------------
#include "motion_global.h"
///-----------------------------------------------------------------------------
#include "../direct/direct.h"
#include "../../../src/motion/common.h"
#include "../../../src/server/server.h"
#include "../../../src/motion/hirestimer.h"
///-----------------------------------------------------------------------------
typedef int  CONSOLE_HANDLER(const char *buf);
typedef void ERRMSG_HANDLER(const char *ErrMsg);
///-----------------------------------------------------------------------------
class MOTIONSHARED_EXPORT MotionClass : public QObject
{
        Q_OBJECT
    public slots:
        void start(const QString &);
    private:
        QMutex* _mutex;
        bool    _pipeOpen;
        bool    _serverMessDisplayed;

        CONSOLE_HANDLER *consoleHandler;
        ERRMSG_HANDLER *errMsgHandler;

        int pipeCmd(int code);
        int pipeCmdStr(int code,const char *s);
        int pipe(const char *s,int n,char *r,int *m);
        int launchServer();

        QString ExtractPath(QString InFile);
        //QFile   PipeFile;

    public:
        QDBusInterface *iface;

        MotionClass(int boardId);
        MotionClass(const MotionClass&) = delete;
        virtual ~MotionClass();
        int     _boardID;

        int     writeLineReadLine(const char *s,char *response);
        int     writeLine(const char *s);
        int     writeLineWithEcho(const char *s);
        int     readLineTimeOut(char *buf,int TimeOutms = 1000000);
        int     listLocations(int *nlocations,int *list);
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


};
///-----------------------------------------------------------------------------
#endif /// MOTION_H
///-----------------------------------------------------------------------------


