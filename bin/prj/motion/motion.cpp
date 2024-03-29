///-----------------------------------------------------------------------------
#include "motion.h"
///-----------------------------------------------------------------------------
extern QString MainPathDLL;
extern QString MainPath;
extern QString MainPathRoot;

void errorMessageBox(const QString);
void errorMessageBox(const QString,QString);

void errorMessageBox(const QString msg)
{
    ///QMessageBox::information(nullptr,"Hello",msg,"Ok");
}
void errorMessageBox(const QString msg,QString header)
{
    ///QMessageBox::information(nullptr,"Hello",msg,"Ok");
}
///-----------------------------------------------------------------------------
MotionClass::MotionClass(int boardID)
{
    _boardID  = boardID;
    _pipeOpen = false;
    _serverMessDisplayed = false;
    _errMessageDisplayed = false;

    _mutex = new QMutex();

    consoleHandler = nullptr;
    errMsgHandler = nullptr;
}
///-----------------------------------------------------------------------------
MotionClass::~MotionClass()
{
///    LPTSTR lpszPipename = "\\\\.\\pipe\\kmotionpipe";
    if (_pipeOpen)
    {
        _pipeOpen = false;
///        if(share==2)
///        {
///            PipeFile.Close();
///            Sleep(100);  // give some time for Server to close
///        }
    }
    delete _mutex;
}
///-----------------------------------------------------------------------------
int MotionClass::writeLineReadLine(const char *s, char *response)
{
    /// Send Code, board, string -- Get Dest (byte), Result (int), and string
    char d[MAX_LINE + 1];
    char r[MAX_LINE + 1];
    int m;
    int result;
    int code = ENUM_WriteLineReadLine;

    result = waitToken("WriteLineReadLine");
    if(result)
    {
        return result;
    }
    memcpy(d,&code,4);
    memcpy(d + 4,&_boardID,4);
    strcpy(d + 8,s);
    pipe(d,strlen(s) + 1 + 8,r,&m);
    memcpy(&result,r + 1,4);
    strcpy(response,r + 1 + 4);
    releaseToken();
    return result;
}
///-----------------------------------------------------------------------------
int MotionClass::writeLine(const char *s)
{
    return pipeCmdStr(ENUM_WriteLine,s);
}
///-----------------------------------------------------------------------------
int MotionClass::writeLineWithEcho(const char *s)
{
    return pipeCmdStr(ENUM_WriteLineWithEcho,s);
}
///-----------------------------------------------------------------------------
int MotionClass::readLineTimeOut(char *response, int TimeOutms)
{
    /// Send Code, _boardID, timeout -- Get Dest, Result (int), and string
    char d[MAX_LINE + 1];
    char r[MAX_LINE + 1];
    int m;
    int result;
    int code = ENUM_ReadLineTimeOut;
    memcpy(d,&code,4);
    memcpy(d + 4,&_boardID,4);
    memcpy(d + 8,&TimeOutms,4);
    pipe(d,12,r,&m);
    memcpy(&result,r + 1,4);
    strcpy(response,r + 1 + 4);
    return result;
}
///-----------------------------------------------------------------------------
int MotionClass::listLocations(int *nlocations, int *list)
{
    /// Send Code -- Get Dest, Result (int), nlocations (int), List (ints)
    char d[MAX_LINE + 1];
    char r[MAX_LINE + 1];
    int m;
    int result;
    int code = ENUM_ListLocations;

    memcpy(d,&code,4);
    pipe(d,4,r,&m);
    memcpy(&result,r+1,4);
    if (result==0)
    {
        memcpy(nlocations,r+1+4,4);
        memcpy(list,r + 1 + 8,(*nlocations * sizeof(int)));
    }
    else
    {
        *nlocations=0;
    }
    return result;
}
///-----------------------------------------------------------------------------
int MotionClass::failed()
{
    return pipeCmd(ENUM_Failed);
}
///-----------------------------------------------------------------------------
int MotionClass::disconnect()
{
    return pipeCmd(ENUM_Disconnect);
}
///-----------------------------------------------------------------------------
int MotionClass::firmwareVersion()
{
    return pipeCmd(ENUM_FirmwareVersion);
}
///-----------------------------------------------------------------------------
int MotionClass::checkForReady()
{
    return pipeCmd(ENUM_CheckForReady);
}
///-----------------------------------------------------------------------------
int MotionClass::motionLock(char *CallerID)
{
    return pipeCmdStr(ENUM_KMotionLock,CallerID);
}
///-----------------------------------------------------------------------------
int MotionClass::usbLocation()
{
    return pipeCmd(ENUM_USBLocation);
}
///-----------------------------------------------------------------------------
int MotionClass::motionLockRecovery()
{
    return pipeCmd(ENUM_KMotionLockRecovery);
}
///-----------------------------------------------------------------------------
/// Try and get the token for the Board
///    return with the token (return value = KMOTION_LOCKED)
/// OR
///    if there is a problem with the board
///    display a message (return value = KMOTION_NOT_CONNECTED)
int MotionClass::waitToken(bool display_msg, int TimeOut_ms, char *CallerID)
{
    HiResTimerClass Timer;
    int result;
    int count(0);
    if(_errMessageDisplayed)
    {
        return SE_MOTION_NOT_CONNECTED;
    }
    Timer.Start();

    do
    {
        /// this Mutex helps maintain a waiting list
        /// so everybody gets a chance at the token
        /// rather than leaving it random.  Also make
        /// sure we have everything before we proceed
        /// so we don't get stuck somewhere (deadlocked)
        if(!_mutex->tryLock(TimeOut_ms))
        {
            return SE_MOTION_IN_USE;
        }
        if(Timer.Elapsed_Seconds() > 2.0 * TimeOut_ms * 0.001)
        {
            _mutex->unlock();
            return SE_MOTION_IN_USE;
        }
        if(count++)
        {
///            Sleep(10);
        }
        result = motionLock(CallerID);
        if(result == SE_MOTION_IN_USE)
        {
            _mutex->unlock();
        }
    }
    while(result == SE_MOTION_IN_USE);

    if(result == SE_MOTION_NOT_CONNECTED && display_msg)
    {
        char s[256];
        if(_boardID > 0)
        {
            sprintf(s,"Can't Connect to SEMotion Board 0x%X",_boardID);
        }
        else
        {
            sprintf(s,"Can't Connect to SEMotion Board #%d",_boardID);
        }
        DoErrMsg(s);
    }
    if(result != SE_MOTION_LOCKED)
    { /// keep the pipe if we have the token
        _mutex->unlock();
    }
    return result;
}
///-----------------------------------------------------------------------------
void MotionClass::releaseToken()
{
    pipeCmd(ENUM_ReleaseToken);
    _mutex->unlock();      // also release the pipe
}
///-----------------------------------------------------------------------------
int  MotionClass::ServiceConsole()
{
    return pipeCmd(ENUM_ServiceConsole);
}
///-----------------------------------------------------------------------------
int MotionClass::SetConsoleCallback(CONSOLE_HANDLER *ch)
{
    consoleHandler = ch;
    /// tell the server who is the server for the console
    return pipeCmd(ENUM_SetConsole);
}
///-----------------------------------------------------------------------------
int MotionClass::SetErrMsgCallback(ERRMSG_HANDLER *ch)
{
    errMsgHandler = ch;
    return 0;
}
///-----------------------------------------------------------------------------
/// send code, board
int MotionClass::pipeCmd(int code)
{
    char d[MAX_LINE + 1];
    char r[MAX_LINE + 1];
    int result;
    int m;

    memcpy(d,&code,4);
    memcpy(d+4,&_boardID,4);
    pipe(d, 8 ,r, &m);
    memcpy(&result,r+1,4);
    return result;
}
///-----------------------------------------------------------------------------
/// send code, board, string
int MotionClass::pipeCmdStr(int code, const char *s)
{
    char d[MAX_LINE + 1];
    char r[MAX_LINE + 1];
    int result;
    int m;

    memcpy(d,&code,4);
    memcpy(d + 4,&_boardID,4);
    if(s == nullptr)
    {
        d[8] = 0;
        pipe(d,1 + 8,r,&m);
    }
    else
    {
        strcpy(d + 8,s);
        pipe(d,static_cast<int>(strlen(s)) + 1 + 8,r,&m);
    }
    memcpy(&result,r+1,4);
    return result;
}
///-----------------------------------------------------------------------------
int MotionClass::pipe(const char *s, int n, char *r, int *m)
{
#if 0
    unsigned char Reply = 0xAA;
    QString ErrorMsg;
    bool ReceivedErrMsg = false;
    static int EntryCount = 0;

    if(_serverMessDisplayed)
        return 1;

    ///    LPTSTR lpszPipename = "\\\\.\\pipe\\kmotionpipe";
    try
    {
        _mutex->lock();
        if(EntryCount > 0)
        {
            int Result = SE_MOTION_IN_USE;
            memcpy(r + 1,&Result,sizeof(Result));
            _mutex->unlock();
            return 1;
        }
        EntryCount++;
        if(!_pipeOpen)
        {
            int i;
            _pipeOpen = true;  // only try once
            if(!PipeFile.Open(lpszPipename, CFile::modeReadWrite))
            {
                // pipe won't open try to launch server
                launchServer();

                for(i=0; i<100; i++) // try for a few secs
                {
                    if (PipeFile.Open(lpszPipename, CFile::modeReadWrite))
                        break;

                    Sleep(100);
                }
                if(i==100)
                {
                    EntryCount--;
                    if (_serverMessDisplayed) return 1;
                    _serverMessDisplayed=TRUE;
                    DoErrMsg("Unable to Connect to KMotion Server");
                    _mutex->Unlock();
                    exit(1);
                }
            }
        }

        PipeFile.Write(s,n); /// Send the request

        for (;;)
        {
            *m = PipeFile.Read(r, MAX_LINE+1);     // Get the response

            // the first byte of the response is the destination
            // currently DEST_NORMAL, DEST_CONSOLE

            if (*r == DEST_CONSOLE)
            {
                PipeFile.Write(&Reply, 1);     // Send an ACK back to server

                // send it to the console if someone registered a callback

                if (consoleHandler)
                    consoleHandler(r+1);
            }
            else if (*r == DEST_ERRMSG)
            {
                PipeFile.Write(&Reply, 1);     // Send an ACK back to server

                // because callback might throw an exception, delay doing the User Callback
                // until everything is received back from the Server and we clean up

                ErrorMsg=r+1;
                ReceivedErrMsg=true;
            }
            else
            {
                break;
            }
        }

        EntryCount--;

        _mutex->Unlock();
    }
    catch (CFileException *e)
    {
        e->Delete();  // to avoid memory leak
        EntryCount--;
        if (_serverMessDisplayed) return 1;
        _serverMessDisplayed=TRUE;
        DoErrMsg("Unable to Connect to KMotion Server");
        _mutex->Unlock();
        exit(1);
    }

    if (ReceivedErrMsg)
    {
        DoErrMsg(ErrorMsg);
    }
#endif
    return 0;
}
///-----------------------------------------------------------------------------
int MotionClass::launchServer()
{
#if 0
    SECURITY_ATTRIBUTES sa          = {0};
    STARTUPINFO         si          = {0};
    PROCESS_INFORMATION pi          = {0};
    HANDLE              hPipeOutputRead  = NULL;
    HANDLE              hPipeOutputWrite = NULL;
    HANDLE              hPipeInputRead   = NULL;
    HANDLE              hPipeInputWrite  = NULL;
    BOOL                bTest = 0;
    DWORD               dwNumberOfBytesRead = 0;

    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;




    // Create pipe for standard output redirection.
    Createpipe(&hPipeOutputRead,  // read handle
        &hPipeOutputWrite, // write handle
        &sa,      // security attributes
        0      // number of bytes reserved for pipe - 0 default
        );

    // Create pipe for standard input redirection.
    Createpipe(&hPipeInputRead,  // read handle
        &hPipeInputWrite, // write handle
        &sa,      // security attributes
        0      // number of bytes reserved for pipe - 0 default
        );

    // Make child process use hPipeOutputWrite as standard out,
    // and make sure it does not show on screen.
    si.cb = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput   = hPipeInputRead;
    si.hStdOutput  = hPipeOutputWrite;
    si.hStdError   = hPipeOutputWrite;

    CString cmd;  // build command line

    cmd = MainPathDLL + "\\KMotionServer.exe";

    if (!CreateProcess (
        NULL,
        cmd.GetBuffer(0),
        NULL, NULL,
        TRUE, 0,
        NULL, NULL,
        &si, &pi))
    {
        _serverMessDisplayed = true;
        CString Errmsg;
        Errmsg.Format("Unable to execute:\r\r" + cmd + "\r\rTry re-installing software or copy this file to the same location as KMotion.exe or Calling Application");
        DoErrMsg(Errmsg);
        exit(1);
    }


    // Now that handles have been inherited, close it to be safe.
    // You don't want to read or write to them accidentally.
    CloseHandle(hPipeOutputWrite);
    CloseHandle(hPipeInputRead);
#endif
    return 0;
}
///-----------------------------------------------------------------------------
QString MotionClass::ExtractPath(QString InFile)
{
#if 0
    int next_pos=0,pos;

    CString OutFile;

    do
    {
        pos=next_pos;
        next_pos = InFile.Find('\\',pos+1);
    }
    while (next_pos!=-1);

    if (pos>0)
        OutFile = InFile.Left(pos);
    else
        OutFile = "";

    return OutFile;
#endif
}
///-----------------------------------------------------------------------------
int MotionClass::CheckKMotionVersion(int *type, bool GetBoardTypeOnly)
{
#if 0
    int result;
    CString BoardVersion;
    CString CoffVersion;
    CString OutFile;
    CString ms;

    if (type) *type = BOARD_TYPE_UNKNOWN;

    if (KMotionLock("CheckKMotionVersion") == KMOTION_LOCKED)  // see if we can get access
    {
        // Get the firmware date from the KMotion Card which
        // will be in PT (Pacific Time)
        releaseToken();
        result = WriteLineReadLine("Version",BoardVersion.GetBufferSetLength(MAX_LINE));
        BoardVersion.ReleaseBuffer();
        if (result) return result;

        // now get date stamp from firmware .out file

        if (BoardVersion.Find("KFLOP")==0)
        {
            OutFile = MainPathRoot +"\\DSP_KFLOP\\DSPKFLOP.out";
            if (type) *type = BOARD_TYPE_KFLOP;
        }
        else
        {
            OutFile = MainPathRoot +"\\DSP_KMotion\\DSPKMotion.out";
            if (type) *type = BOARD_TYPE_KMOTION;
        }
        if (GetBoardTypeOnly) return 0;

        int result = ExtractCoffVersionString(OutFile,CoffVersion.GetBuffer(81));
        CoffVersion.ReleaseBuffer();

        if (result)
        {
            ms.Format("Error Extracting Version Information from file\r\r"
                " %s",
                OutFile.GetBuffer(0));

            DoErrMsg(ms);
            return 1;
        }

        CoffVersion.Remove('\n');


        // check if they match exactly

        if (CoffVersion == BoardVersion) return 0;

        if (BoardVersion.Find("KFLOP")==0)
            ms.Format("DSP_KFLOP.out Date Stamp Doesn't match KFLOP Firmware\r\r"
                " Before compiling programs please use Flash/Config Screen and select:\r"
                " Download New Version.  This will install compatible Firmware with\r"
                " this version of software\r\r"
                " %s  KFLOP Firmware\r"
                " %s  DSP_KFLOP.out file",
                BoardVersion.GetBuffer(0),
                CoffVersion.GetBuffer(0));
        else
            ms.Format("DSP_KMotion.out Date Stamp Doesn't match KMotion Firmware\r\r"
                " Before compiling programs please use Flash/Config Screen and select:\r"
                " Download New Version.  This will install compatible Firmware with\r"
                " this version of software\r\r"
                " %s  KMotion Firmware\r"
                " %s  DSP_KMotion.out file",
                BoardVersion.GetBuffer(0),
                CoffVersion.GetBuffer(0));



        DoErrMsg(ms);

        return 1;
    }
#endif
    return 0;
}
///-----------------------------------------------------------------------------
void MotionClass::DoErrMsg(const char *s)
{
#if 0
    if (!_errMessageDisplayed)
    {
        _errMessageDisplayed=true;
        if (errMsgHandler)
        {
            __try
            {
                errMsgHandler(s);
            }
            __finally
            {
                _errMessageDisplayed=false;
            }
        }
        else
        {
            AfxMessageBox(s,MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
        }
        _errMessageDisplayed=false;
    }
#endif
}
///-----------------------------------------------------------------------------
int MotionClass::getStatus(MAIN_STATUS& status, bool lock)
{
#if 0
    int i,result,n,token;
    QString s;
    int *p=(int *)&status;
    if(lock)
    {
        token = waitToken( false, 100, "GetStatus");
        if (token != SE_MOTION_LOCKED) return 1;
    }

    // KMotion is available read the status
    s = QString("GetStatus");
    ///s.Format("GetStatus");
    if (WriteLine(s))
    {
        if (lock) releaseToken();
        return 1;
    }

    n=sizeof(status)/sizeof(int);

    ///s.Empty();
    s.clear();

    for (i=0; i<n; i++)
    {
        if (s.isEmpty())
        {
            if (ReadLineTimeOut(s.GetBuffer(257),5000))
            {
                if (lock) releaseToken();
                return 1;
            }

            s.ReleaseBuffer();

            // change the CRLF at the to a space

            s.Delete(s.GetLength()-2,2);

            s += ' ';
        }

        // get a hex 32 bit int which may really be anything

        result = sscanf(s.GetBuffer(0),"%8X",p++);

        if (result!=1)
        {
            if (lock) releaseToken();
            return 1;
        }

        if (s.GetLength() >=9)
        {
            s.Delete(0,9);
        }
        else
        {
            if (lock) releaseToken();
            return 1;
        }

        // check if first word contains version

        if (i==0)
        {
            if ((p[-1]>>16)==306)  // check for previous version
            {
            }

            if ((p[-1]>>16)==0)
            {
                // probably old version of DSP Code

                // set version to something

                status.VersionAndSize=0;
                status.ThreadActive=0;

                p[0] = p[-1];
                 p++;
                n=n-2;
            }
        }
    }
    if(lock)releaseToken();
#endif
    return 0;
}
///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
/// send message from motion to server
///int pipe(const char *s, int n, char *r, int *m)
///{
///
///    unsigned char Reply = 0xAA;
///    QString ErrorMsg;
///    bool ReceivedErrMsg = false;
///    static int EntryCount = 0;
///
///    if(_serverMessDisplayed)
///        return 1;
///
///    try
///    {
///        _mutex->lock();
///        EntryCount++;
///        PipeFile.Write(s,n); /// Send the request
///        for (;;)
///        {
///            /// ответ от сервера
///            /// *m - количество прочитаных байт в буфере r
///            *m = PipeFile.Read(r, MAX_LINE+1);     // Get the response
///            /// парсинг первого байта ответа
///            // the first byte of the response is the destination
///            // currently DEST_NORMAL, DEST_CONSOLE
///            if(*r == DEST_CONSOLE)
///            {
///                PipeFile.Write(&Reply, 1);     // Send an ACK back to server
///                // send it to the console if someone registered a callback
///                if (consoleHandler)
///                    consoleHandler(r+1);
///            }
///            else
///            if(*r == DEST_ERRMSG)
///            {
///                PipeFile.Write(&Reply, 1);     // Send an ACK back to server
///                // because callback might throw an exception, delay doing the User Callback
///                // until everything is received back from the Server and we clean up
///                ErrorMsg=r+1;
///                ReceivedErrMsg=true;
///            }
///            else
///            {
///                break;
///            }
///        }
///        EntryCount--;
///        _mutex->Unlock();
///    }
///
///    catch (CFileException *e)
///    {
///        e->Delete();  // to avoid memory leak
///        EntryCount--;
///        if (_serverMessDisplayed) return 1;
///        _serverMessDisplayed=TRUE;
///        DoErrMsg("Unable to Connect to KMotion Server");
///        _mutex->Unlock();
///        exit(1);
///    }
///
///    if (ReceivedErrMsg)
///    {
///        DoErrMsg(ErrorMsg);
///    }
///    return 0;
///}
///-----------------------------------------------------------------------------
