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
#define NO_SEMOTION_TIMEOUT false // useful for debugging
///-----------------------------------------------------------------------------
typedef int SERVER_CONSOLE_HANDLER (int board, const char *buf);
///-----------------------------------------------------------------------------
class MotionIOClass
{
    public:
        bool    FailMessageAlreadyShown;
        bool    SendAbortOnConnect;
        int     LogToConsole(char *s);
        int     HandleDiskIO(char *s);
        int     SEMotionLock(char *CallerID);
        int     USBLocation();
        int     SEMotionLockRecovery();
        int     MakeSureConnected();
        void    ReleaseToken();
        int     CheckForReady();
        int     Failed();
        int     Disconnect();
        int     FirmwareVersion();
        int     NonRespondingCount;
        int     FlushInputBuffer();
        QMutex *Mutex;
        int     NumberBytesAvailToRead(int *navail,bool ShowMessage);
        int     WriteLineReadLine(const char *send,char *response);
        bool    RequestedDeviceAvail(QString *reason);
        int     ReadLineTimeOut(char *buf,int TimeOutms);
        int     ReadLineTimeOutRaw(char *buf,int TimeOutms);
        int     SetLatency(unsigned char LatencyTimer);
        int     WriteLineWithEcho(const char * s);
        int     WriteLine(const char *s);
        int     ReadBytesAvailable(char *RxBuffer,int maxbytes,DWORD *BytesReceived,int timeout_ms);
        int     ReadSendNextLine(FILE *fr);
        int     ServiceConsole();
        int     SetConsoleCallback(SERVER_CONSOLE_HANDLER *ch);
        int     Connect();
        MotionIOClass();
        virtual ~MotionIOClass();

        bool    BoardIDAssigned;
        int     USB_Loc_ID;
        bool    m_Connected;
        QString ErrMsg;

        QString m_LastCallerID;

    protected:
        int     Token;
        char    m_SaveChars[MAX_LINE + 1];
        FT_HANDLE   ftHandle;
        SERVER_CONSOLE_HANDLER *ConsoleHandler;
    private:
        int ErrorMessageBox(const char *s);
        int m_FirmwareVersion;

        /// my implamentation
        DWORD timeGetTime(void);
        void Sleep(DWORD millseconds);
        void timeBeginPeriod(DWORD milliseconds);
        void timeEndPeriod(DWORD milliseconds);
};
///-----------------------------------------------------------------------------
#endif /// MOTION_IO_H
///-----------------------------------------------------------------------------
