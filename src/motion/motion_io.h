///-----------------------------------------------------------------------------
#ifndef MOTION_IO_H
#define MOTION_IO_H
///-----------------------------------------------------------------------------
#include <QString>
#include <QMutex>
///-----------------------------------------------------------------------------
#include "../ftdi/ftd2xx.h"
#include "../dsp/PC-DSP.h"
///-----------------------------------------------------------------------------
#include "common.h"
///-----------------------------------------------------------------------------
namespace MOTION_DIRECT_SPACE
{
    /**
     * @brief MAX_LINE buffer max size
     */
    const unsigned short BUFFER_MAX_LINE = 2560;

    ///-------------------------------------------------------------------------
    #define NO_SEMOTION_TIMEOUT false /// useful for debugging
    ///-------------------------------------------------------------------------
    typedef int SERVER_CONSOLE_HANDLER(const char *buf);
    ///-------------------------------------------------------------------------
    class MotionIOClass
    {
        private:
            QMutex *_mutex;
            QString _serialNumber;
            int     _nonRespondingCount;
            int     _firmwareVersion;
            bool    _failMessageAlreadyShown;
            bool    _sendAbortOnConnect;
            bool    _connected;

            int     errorMessageBox(QString s);
            DWORD   getCurrentTimeMs(void);
            int     sleep(DWORD miliseconds);
            bool    requestedDeviceAvail(QString *reason);
            int     connect();
            int     setLatency(unsigned char LatencyTimer);
            int     flushInputBuffer();
            int     logToConsole(char *s);
            int     handleDiskIO(char *s);
            int     makeSureConnected();
            int     numberBytesAvailToRead(int *navail,bool ShowMessage);
            int     readLineTimeOutRaw(char *buf,int TimeOutms);
            int     readBytesAvailable(char *RxBuffer,int maxbytes,DWORD *BytesReceived,int timeout_ms = 0);
            int     readSendNextLine(FILE *fr);

            void inline charToUpper(char* mass,unsigned int length);
        protected:
            int     _token;
            char    _saveChars[BUFFER_MAX_LINE + 1];
            FT_HANDLE   _ftHandle;
            SERVER_CONSOLE_HANDLER *ConsoleHandler;
        public:
            QString _errMsg;
            QString _lastCallerID;
            bool    _boardAssigned;

            MotionIOClass();
            MotionIOClass(const MotionIOClass&) = delete; /// disable copy constructor
            void operator=(const MotionIOClass&)= delete; /// desable overload operator=
            virtual ~MotionIOClass();
            QString                 getLastErrorMessage(void);
            SE_MOTION_LOCK_STATE    motionLock(char *CallerID);
            SE_MOTION_LOCK_STATE    motionLockRecovery();
            QString                 usbLocation();
            void                    releaseToken();
            SE_MOTION_STATE         checkForReady();
            int                     failed();
            int                     disconnect();
            int                     firmwareVersion();
            int                     writeLineReadLine(const char *send,char *response);
            int                     readLineTimeOut(char *buf,int TimeOutms);
            int                     writeLineWithEcho(const char * s);
            int                     writeLine(const char *s);
            int                     serviceConsole();
            int                     setConsoleCallback(SERVER_CONSOLE_HANDLER *ch);
    };
} /// end MOTION_DIRECT_SPACE
///-----------------------------------------------------------------------------
#endif /// MOTION_IO_H
///-----------------------------------------------------------------------------
