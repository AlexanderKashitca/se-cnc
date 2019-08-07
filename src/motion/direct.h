///-----------------------------------------------------------------------------
#ifndef DIRECT_H
#define DIRECT_H
///-----------------------------------------------------------------------------
#include <QLibrary>
#include "commands.h"
#include "../../../src/motion/motion_io.h"
///-----------------------------------------------------------------------------
namespace MOTION_DIRECT_SPACE
{
    class MotionDirectClass
    {
        private:
            MotionIOClass MotionIO;
            QLibrary library;
            bool    _ftdiLibraryLoad;
        public:
            MotionDirectClass();
            MotionDirectClass(const MotionDirectClass&) = delete;
            ~MotionDirectClass();
            bool    getFtdiLibraryLoad();

            QString usbLocation();
            int     listLocations(int *nlocations,int *list);
            SE_MOTION_LOCK_STATE     motionLock(char *CallerID = nullptr);
            SE_MOTION_LOCK_STATE     motionLockRecovery();
            void    releaseToken(); /// ???
            SE_MOTION_STATE checkForReady();
            int     failed();
            int     disconnect();


            ///int     mapBoardToIndex(int BoardID);
            int     writeLine(const char *s);
            int     writeLineReadLine(const char *s,char *response);
            int     writeLineWithEcho(const char *s);
            int     readLineTimeOut(char *buf,int TimeOutms);
            ///void    waitToken(char *CallerID = nullptr);




            int     firmwareVersion();

            int     serviceConsole();
            const char* getErrMsg();
            void    clearErrMsg();
            int     setConsoleCallback(SERVER_CONSOLE_HANDLER *ch);
            int     nInstances();
    };
}
///-----------------------------------------------------------------------------
#endif /// DIRECT_H
///-----------------------------------------------------------------------------
