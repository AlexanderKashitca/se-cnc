///-----------------------------------------------------------------------------
#include <QList>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>
///-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
///-----------------------------------------------------------------------------
#include "../server/server.h"
#include "../direct/direct.h"
///-----------------------------------------------------------------------------
/// the property
QString ServerClass::value() const
{
    return m_value;
}
///-----------------------------------------------------------------------------
void ServerClass::setValue(const QString &newValue)
{
    m_value = newValue;
}
///-----------------------------------------------------------------------------
void ServerClass::quit()
{
    QTimer::singleShot(0,QCoreApplication::instance(),&QCoreApplication::quit);
}
///-----------------------------------------------------------------------------
QDBusVariant ServerClass::query(const int &query)
{



}
///-----------------------------------------------------------------------------
QDBusVariant ServerClass::query(const QString &query)
{
    QString q = query.toLower();
    if (q == "hello")
        return QDBusVariant("World");
    if (q == "ping")
        return QDBusVariant("Pong");
    if (q.indexOf("the answer to life, the universe and everything") != -1)
        return QDBusVariant(42);
    if (q.indexOf("unladen swallow") != -1) {
        if (q.indexOf("european") != -1)
            return QDBusVariant(11.0);
        return QDBusVariant(QByteArray("african or european?"));
    }

    return QDBusVariant("Sorry, I don't know the answer");
}
///-----------------------------------------------------------------------------






///-----------------------------------------------------------------------------
#if 0
#define BUFSIZE     4096
#define MAX_BOARDS  16
///-----------------------------------------------------------------------------
static MotionDirectClass MotionDirect;
///-----------------------------------------------------------------------------
static void GetAnswerToRequest(char *chRequest,
                        unsigned int nInBytes,
                        char *chReply,
                        unsigned int *cbReplyBytes,
                        void* hPipe);
static int ConsoleHandler(int board,const char *buf);
///-----------------------------------------------------------------------------
static void* ConsolePipeHandle[MAX_BOARDS];
static QList <QString> ConsoleList[MAX_BOARDS];
///-----------------------------------------------------------------------------
void MyErrExit(char *s)
{
///	MessageBox(NULL,s,"KMotion Server",MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL);
    exit(1);
}
///-----------------------------------------------------------------------------
///extern CMainFrame *TheFrame;

#if 0

#define PIPE_TIMEOUT 10000

void InstanceThread(LPVOID); 
 
int nClients = 0; 



#endif
#endif
void ServerMain(LPVOID lpvParam) 
{ 
   BOOL fConnected; 
   DWORD dwThreadId; 
   HANDLE hPipe, hThread; 
   LPTSTR lpszPipename = "\\\\.\\pipe\\kmotionpipe"; 

   for (int i=0; i<MAX_BOARDS; i++) ConsolePipeHandle[i]=NULL;
 
// The main loop creates an instance of the named pipe and 
// then waits for a client to connect to it. When the client 
// connects, a thread is created to handle communications 
// with that client, and the loop is repeated. 
 
   for (;;) 
   { 
      hPipe = CreateNamedPipe( 
          lpszPipename,             // pipe name 
          PIPE_ACCESS_DUPLEX,       // read/write access 
          PIPE_TYPE_MESSAGE |       // message type pipe 
          PIPE_READMODE_MESSAGE |   // message-read mode 
          PIPE_WAIT,                // blocking mode 
          PIPE_UNLIMITED_INSTANCES, // max. instances  
          BUFSIZE,                  // output buffer size 
          BUFSIZE,                  // input buffer size 
          PIPE_TIMEOUT,             // client time-out 
          NULL);                    // no security attribute 

      if (hPipe == INVALID_HANDLE_VALUE) 
          MyErrExit("CreatePipe"); 
 
      // Wait for the client to connect; if it succeeds, 
      // the function returns a nonzero value. If the function returns 
      // zero, GetLastError returns ERROR_PIPE_CONNECTED. 
 
      fConnected = ConnectNamedPipe(hPipe, NULL) ? 
         TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
 
      if (fConnected) 
      { 

		  nClients++;

      // Create a thread for this client. 
         hThread = CreateThread( 
            NULL,              // no security attribute 
            0,                 // default stack size 
            (LPTHREAD_START_ROUTINE) InstanceThread, 
            (LPVOID) hPipe,    // thread parameter 
            0,                 // not suspended 
            &dwThreadId);      // returns thread ID 

         if (hThread == NULL) 
            MyErrExit("CreateThread"); 
 
      } 
      else 
        // The client could not connect, so close the pipe. 
         CloseHandle(hPipe); 
   } 
   return; 
} 
///#endif
VOID InstanceThread(void* lpvParam)
{ 
   char chRequest[BUFSIZE];
   char chReply[BUFSIZE];
   unsigned int cbBytesRead;
   unsigned int cbReplyBytes;
   unsigned int cbWritten;
   bool fSuccess;
   void* hPipe;
 
// The thread's parameter is a handle to a pipe instance. 
 
	hPipe = (HANDLE) lpvParam; 

	while (1) 
	{ 
///		// Read client requests from the pipe.
///		fSuccess = ReadFile(
///			 hPipe,        // handle to pipe
///			 chRequest,    // buffer to receive data
///			 BUFSIZE,      // size of buffer
///			 &cbBytesRead, // number of bytes read
///			 NULL);        // not overlapped I/O

      if (! fSuccess || cbBytesRead == 0) 
         break; 
      
      GetAnswerToRequest(chRequest,cbBytesRead,chReply,&cbReplyBytes,hPipe);
	   
   // Write the reply to the pipe. 
///      fSuccess = WriteFile(
///         hPipe,        // handle to pipe
///         chReply,      // buffer to write from
///         cbReplyBytes, // number of bytes to write
///         &cbWritten,   // number of bytes written
///         NULL);        // not overlapped I/O

      if (! fSuccess || cbReplyBytes != cbWritten) break; 
  } 
 
// Flush the pipe to allow the client to read the pipe's contents 
// before disconnecting. Then disconnect the pipe, and close the 
// handle to this pipe instance. 
 
///   FlushFileBuffers(hPipe);
///   DisconnectNamedPipe(hPipe);
///   CloseHandle(hPipe);


///   if (--nClients <= 0) exit(0);              // nobody left - terminate server
   if (MotionDirect.nInstances() < 2) exit(0);  // nobody left - terminate server
} 
 

///-----------------------------------------------------------------------------
void GetAnswerToRequest(char *chRequest,
                        unsigned int nInBytes,
                        char *chReply,
                        unsigned int *cbReplyBytes,
                        void* hPipe)
{
    int code;
    int board;
    int BoardID;
    int result(0);
    int List[256];
    int TimeOutms;
    int nLocations;



	memcpy(&code, chRequest,4);

	if (code!=ENUM_ListLocations)  // all commands require a board to be mapped, except this command
	{
		memcpy(&BoardID, chRequest+4,4);
///        board=MotionDirect.MapBoardToIndex(BoardID);
	}

	chReply[0]=DEST_NORMAL;
	switch (code)
	{
        case ENUM_WriteLineReadLine:	// Send Code, board, string -- Get Dest byte, Result (int) and string
            result = MotionDirect.writeLineReadLine(chRequest+8, chReply+1+4);
            memcpy(chReply+1, &result,4);
            *cbReplyBytes = 1+4+strlen(chReply+1+4)+1; // Dest byte, Result int, string, null char
            break;
        case ENUM_WriteLine:
            result = MotionDirect.writeLine(chRequest+8);
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_WriteLineWithEcho:
            result = MotionDirect.writeLineWithEcho(chRequest+8);
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_ReadLineTimeOut:	// Send Code, board, timeout -- Dest byte, Get Result (int), and string
            memcpy(&TimeOutms, chRequest+8,4);
            result = MotionDirect.readLineTimeOut(chReply+1+4 ,TimeOutms);
            memcpy(chReply+1, &result,4);
            *cbReplyBytes = 1+4+strlen(chReply+1+4)+1; // Dest byte, Result int, string, null char
            break;
        case ENUM_ListLocations:		// Send Code -- Get Dest, Result (int), nlocations (int), List (ints)
            result = MotionDirect.listLocations(&nLocations, List);
            memcpy(chReply+1, &result,4);
            memcpy(chReply+1+4, &nLocations,4);
            memcpy(chReply+1+8, List, nLocations*4);
            *cbReplyBytes = 1+4+4+4*nLocations; // Dest byte, Result int, string, null char
            break;
        case ENUM_Failed:
            result = MotionDirect.failed();
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_Disconnect:
            result = MotionDirect.disconnect();
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_FirmwareVersion:
            result = MotionDirect.firmwareVersion();
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_CheckForReady:
            result = MotionDirect.checkForReady();
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_KMotionLock:
            result = MotionDirect.motionLock(chRequest + 8);
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_USBLocation:
///        result = MotionDirect.usbLocation();
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_KMotionLockRecovery:
            result = MotionDirect.motionLockRecovery();
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_ReleaseToken:
            MotionDirect.releaseToken();
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_ServiceConsole:
            result = MotionDirect.serviceConsole();
            memcpy(chReply+1, &result,4);
            *cbReplyBytes=1+4;
            break;
        case ENUM_SetConsole:
            /// remember which pipe is associated with the console handler for the board
            ///ConsolePipeHandle[board] = hPipe;
            result = MotionDirect.setConsoleCallback(ConsoleHandler);
            ///memcpy(chReply+1, &result,4);
            ///*cbReplyBytes=1+4;
            break;
        default :
            MyErrExit("Bad Request Code");
	}

///	// before we send the answer back check if this pipe
///	// is the message handler for the board and there are
///	// messages in the queue
///	if (code!=ENUM_ListLocations && hPipe == ConsolePipeHandle[board])
///	{
///		int nSent=0;
///
///		while (!ConsoleList[board].IsEmpty() && nSent++<10)
///		{
///			DWORD cbReplyBytes, cbBytesRead, cbWritten;
///			unsigned char Reply;
///			BOOL fSuccess;
///			CString s = ConsoleList[board].RemoveHead();
///
///			s.Insert(0,DEST_CONSOLE);
///
///			cbReplyBytes = s.GetLength()+1+1;  // + Term Null + DEST code
///
///			// Write the message to the pipe.
///			fSuccess = WriteFile(
///				 hPipe,        // handle to pipe
///				 s.GetBuffer(0),      // buffer to write from
///				 cbReplyBytes, // number of bytes to write
///				 &cbWritten,   // number of bytes written
///				 NULL);        // not overlapped I/O
///
///			if (! fSuccess || cbReplyBytes != cbWritten) break;
///
///		   // Read back 1 byte ack 0xAA from Console Handler
///
///			fSuccess = ReadFile(
///				 hPipe,        // handle to pipe
///				 &Reply,    // buffer to receive data
///				 1,      // size of buffer
///				 &cbBytesRead, // number of bytes read
///				 NULL);        // not overlapped I/O
///
///if (! fSuccess || cbBytesRead != 1 || Reply != 0xAA)
///break;
///}
///}

	// check if we have an error message to send back

    if (code!=ENUM_ListLocations && !MotionDirect.getErrMsg()[0]==0)
	{
		DWORD cbReplyBytes, cbBytesRead, cbWritten; 
		unsigned char Reply;
		BOOL fSuccess; 
        QString s = MotionDirect.getErrMsg();

        s.insert(0,DEST_ERRMSG);

        cbReplyBytes = s.length() + 1 + 1;  // + Term Null + DEST code

///		// Write the message to the pipe.
///		fSuccess = WriteFile(
///			 hPipe,        // handle to pipe
///			 s.GetBuffer(0),      // buffer to write from
///			 cbReplyBytes, // number of bytes to write
///			 &cbWritten,   // number of bytes written
///			 NULL);        // not overlapped I/O

///		if (fSuccess && cbReplyBytes == cbWritten)
///		{
///		   // Read back 1 byte ack 0xAA from Console Handler
///
///			fSuccess = ReadFile(
///				 hPipe,        // handle to pipe
///				 &Reply,    // buffer to receive data
///				 1,      // size of buffer
///				 &cbBytesRead, // number of bytes read
///				 NULL);        // not overlapped I/O
///		}

        MotionDirect.clearErrMsg();
	}
}
///-----------------------------------------------------------------------------
int ConsoleHandler(int board, const char *buf)
{
    /// check if there is a console handler for this board
    if(ConsolePipeHandle[board])
	{
        /// there is, add the message to the list
        ConsoleList[board].append(buf);
	}
	return 0;
}
///-----------------------------------------------------------------------------
#endif





/// send message from motion to server
int MotionClass::pipe(const char *s, int n, char *r, int *m)
{

    unsigned char Reply = 0xAA;
    QString ErrorMsg;
    bool ReceivedErrMsg = false;
    static int EntryCount = 0;

    if(_serverMessDisplayed)
        return 1;

    try
    {
        _pipeMutex->lock();
        EntryCount++;
        PipeFile.Write(s,n); /// Send the request
        for (;;)
        {
            /// ответ от сервера
            /// *m - количество прочитаных байт в буфере r
            *m = PipeFile.Read(r, MAX_LINE+1);     // Get the response
            /// парсинг первого байта ответа
            // the first byte of the response is the destination
            // currently DEST_NORMAL, DEST_CONSOLE
            if(*r == DEST_CONSOLE)
            {
                PipeFile.Write(&Reply, 1);     // Send an ACK back to server
                // send it to the console if someone registered a callback
                if (consoleHandler)
                    consoleHandler(r+1);
            }
            else
            if(*r == DEST_ERRMSG)
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
        _pipeMutex->Unlock();
    }

    catch (CFileException *e)
    {
        e->Delete();  // to avoid memory leak
        EntryCount--;
        if (_serverMessDisplayed) return 1;
        _serverMessDisplayed=TRUE;
        DoErrMsg("Unable to Connect to KMotion Server");
        _pipeMutex->Unlock();
        exit(1);
    }

    if (ReceivedErrMsg)
    {
        DoErrMsg(ErrorMsg);
    }
    return 0;
}
