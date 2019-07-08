///-----------------------------------------------------------------------------
#ifndef MOTION_IO_H
#define MOTION_IO_H
///-----------------------------------------------------------------------------
#include <QDebug>
#include <QString>
#include <QMutex>
///-----------------------------------------------------------------------------
#include "../ftdi/ftd2xx.h"
#include "../dsp/PC-DSP.h"
///-----------------------------------------------------------------------------
#ifndef MAX_LINE
    #define MAX_LINE    2560
#endif
///-----------------------------------------------------------------------------
#define NO_SEMOTION_TIMEOUT false /// useful for debugging
///-----------------------------------------------------------------------------
typedef int SERVER_CONSOLE_HANDLER(int board,const char *buf);
///-----------------------------------------------------------------------------
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

        int     errorMessageBox(const char *s);
        int     errorMessageBox(QString const s);
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
    protected:
        int     _token;
        char    _saveChars[MAX_LINE + 1];
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

        int     motionLock(char *CallerID);
        int     motionLockRecovery();
        QString usbLocation();
        void    releaseToken();
        int     checkForReady();
        int     failed();
        int     disconnect();
        int     firmwareVersion();
        int     writeLineReadLine(const char *send,char *response);
        int     readLineTimeOut(char *buf,int TimeOutms);
        int     writeLineWithEcho(const char * s);
        int     writeLine(const char *s);
        int     serviceConsole();
        int     setConsoleCallback(SERVER_CONSOLE_HANDLER *ch);

/*
        int     mapBoardToIndex(int BoardID);
        int     listLocations(int *nlocations,int *list);
        void    waitToken(char *CallerID = nullptr);
        const char* getErrMsg();
        void    clearErrMsg();
        int     nInstances();
 */
};
///-----------------------------------------------------------------------------
#endif /// MOTION_IO_H
///-----------------------------------------------------------------------------
