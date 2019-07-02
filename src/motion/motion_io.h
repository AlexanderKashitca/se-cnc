///-----------------------------------------------------------------------------
#ifndef MOTION_IO_H
#define MOTION_IO_H
///-----------------------------------------------------------------------------
#include <QString>
#include <QMutex>
///-----------------------------------------------------------------------------
#include "../ftdi/ftd2xx.h"
///-----------------------------------------------------------------------------
#include "hirestimer.h"
///-----------------------------------------------------------------------------
#ifndef MAX_LINE
    #define MAX_LINE    2560
#endif
#ifndef MAX_BOARDS
    #define MAX_BOARDS  16
#endif
///-----------------------------------------------------------------------------
extern HiResTimerClass Timer;
///-----------------------------------------------------------------------------
#define NO_SEMOTION_TIMEOUT false /// useful for debugging
///-----------------------------------------------------------------------------
typedef int SERVER_CONSOLE_HANDLER (int board, const char *buf);
///-----------------------------------------------------------------------------
typedef struct
{
    DWORD dwIndex;
    DWORD lpdwFlags;
    DWORD lpdwType;
    DWORD lpdwID;
    DWORD lpdwLocId;
    char  SerialNumber[16];
    char  Description[64];
}FtdiDeviceInfoDetail;
///-----------------------------------------------------------------------------
class MotionIOClass
{
    private:
        bool    requestedDeviceAvail(QString *reason);
        int     connect();
    public:
        bool    FailMessageAlreadyShown;
        bool    SendAbortOnConnect;
        int     logToConsole(char *s);
        int     handleDiskIO(char *s);
        int     motionLock(char *CallerID);
        int     usbLocation();
        int     motionLockRecovery();
        int     makeSureConnected();
        void    releaseToken();
        int     checkForReady();
        int     failed();
        int     disconnect();
        int     firmwareVersion();
        int     NonRespondingCount;
        int     flushInputBuffer();
        QMutex *Mutex;
        int     numberBytesAvailToRead(int *navail,bool ShowMessage);
        int     writeLineReadLine(const char *send,char *response);

        int     readLineTimeOut(char *buf,int TimeOutms);
        int     readLineTimeOutRaw(char *buf,int TimeOutms);
        int     setLatency(unsigned char LatencyTimer);
        int     writeLineWithEcho(const char * s);
        int     writeLine(const char *s);
        int     readBytesAvailable(char *RxBuffer,int maxbytes,DWORD *BytesReceived,int timeout_ms = 0);
        int     readSendNextLine(FILE *fr);
        int     serviceConsole();
        int     setConsoleCallback(SERVER_CONSOLE_HANDLER *ch);

        MotionIOClass();
        virtual ~MotionIOClass();


        bool    m_Connected;
        QString ErrMsg;
        QString m_LastCallerID;

        int     USB_Loc_ID;
        bool    BoardIDAssigned;
private:



    protected:
        int     Token;
        char    m_SaveChars[MAX_LINE + 1];
        FT_HANDLE   ftHandle;
        SERVER_CONSOLE_HANDLER *ConsoleHandler;
    private:
        int errorMessageBox(const char *s);
        int m_FirmwareVersion;
        /// my implamentation
        DWORD getCurrentTimeMs(void);
        int   sleep(DWORD miliseconds);
};
///-----------------------------------------------------------------------------
#endif /// MOTION_IO_H
///-----------------------------------------------------------------------------
