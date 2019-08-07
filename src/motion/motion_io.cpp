///-----------------------------------------------------------------------------
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
///-----------------------------------------------------------------------------
#include "common.h"
#include "motion_io.h"
///-----------------------------------------------------------------------------
#define CONNECT_TRIES       5
#define CONNECT_TIMEOUT     1000
#define TIME_TO_TRY_TO_OPEN 3000
///-----------------------------------------------------------------------------
MotionIOClass::MotionIOClass()
{
    ConsoleHandler = nullptr;
    _mutex = new QMutex();
    _connected = false;
    _sendAbortOnConnect = true;
    _failMessageAlreadyShown = false;
    _token = 0;
    _nonRespondingCount = 0;
    _firmwareVersion = 2;
    _boardAssigned = false;
    _serialNumber.clear();

}
///-----------------------------------------------------------------------------
MotionIOClass::~MotionIOClass()
{
    delete _mutex;
}
///-----------------------------------------------------------------------------
// save the error message to be piped back to caller
int MotionIOClass::errorMessageBox(QString s)
{
    _errMsg = s;
    return 0;
}
///-----------------------------------------------------------------------------
DWORD MotionIOClass::getCurrentTimeMs()
{
    qint64  time_us;
    timeval curTime;
    gettimeofday(&curTime,nullptr);

    time_us  = static_cast<qint64>(curTime.tv_sec * 1000000);
    time_us += curTime.tv_usec;
    time_us /= 1000;
    return(static_cast<DWORD>(time_us));
}
///-----------------------------------------------------------------------------
/// return
int MotionIOClass::sleep(DWORD miliseconds)
{
    struct timespec req;
    struct timespec rem;
    if(miliseconds > 999)
    {
        /// Must be Non-Negative
        req.tv_sec = static_cast<int>(miliseconds / 1000);
        /// Must be in range of 0 to 999999999
        req.tv_nsec = (miliseconds - (static_cast<long>(req.tv_sec) * 1000)) * 1000000;
    }
    else
    {
        /// Must be Non-Negative
        req.tv_sec = 0;
        /// Must be in range of 0 to 999999999
        req.tv_nsec = miliseconds * 1000000;
    }
    return(nanosleep(&req,&rem));
}
///-----------------------------------------------------------------------------
/// return true if successful
bool MotionIOClass::requestedDeviceAvail(QString *Reason)
{
    FT_DEVICE_LIST_INFO_NODE* devInfo(nullptr);
	FT_STATUS ftStatus;
    DWORD     numDevs(0);
    /// create the device information list
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if(ftStatus == FT_OK)
	{ 
        if(numDevs > 0)
		{ 
            /// allocate storage for list based on numDevs
            devInfo = new FT_DEVICE_LIST_INFO_NODE[(sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs)];
            if(devInfo != nullptr)
            {
                /// get the device information list
                ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
                if(ftStatus == FT_OK)
                {
                    if(strstr(devInfo->SerialNumber,"SE#") != nullptr)
                    { /// finding serial number with our signature
                        if(!_boardAssigned)
                        {
                            _boardAssigned = true;
                            _serialNumber.clear();
                            _serialNumber.append(devInfo->SerialNumber);
                        }
                    }
                    else
                    {
                        _serialNumber.clear();
                        if(Reason)
                        {
                            *Reason = QString("Device present, but serial signature not find");
                        }
                        delete (devInfo);
                        return false;
                    }
                }
                else
                {
                    if(Reason)
                    {
                        *Reason = QString("FT_GetDeviceInfoList failed (error code %d)")
                                    .arg(ftStatus);
                    }
                    delete (devInfo);
                    return false;
                }
            }
            else
            {
                if(Reason)
                {
                     *Reason = "Allocation failure";
                }
                return false;
            }
		}
		else
		{
            if(Reason) *Reason ="No SEMotion devices available";
			return false;
		}
        delete (devInfo);
	}
	return true;
}
///-----------------------------------------------------------------------------
int MotionIOClass::connect()
{
    QString    reason;
    QByteArray serial;
    FT_STATUS  ftStatus;

    if(_nonRespondingCount == CONNECT_TRIES)
    {
        return 1;
    }
    _saveChars[0] = 0; /// start anew
    if(!requestedDeviceAvail(&reason))
    {
        errorMessageBox(reason);
        return 1;
    }
    /// FT_ListDevices OK, number of devices connected is in numDevs
    /// usually during boot the board comes and goes, since it appeared
    /// to be there, try for a while to open it
    DWORD t0 = getCurrentTimeMs();
    while(1)
	{
        serial = _serialNumber.toLocal8Bit();
        ftStatus = FT_OpenEx(static_cast<void*>(serial.data()),FT_OPEN_BY_SERIAL_NUMBER,&_ftHandle);
        if(ftStatus == FT_OK)
        { /// FT_Open OK, use _ftHandle to access device
            errorMessageBox("FT_Open succeeded.  Handle is");
            if(setLatency(2))
			{
				return 1;
			}
            if(flushInputBuffer())
			{
                FT_Close(_ftHandle);
				return 1;
			}
            _connected = true;  // All set
			return 0;
		}
		else 
        { /// FT_Open failed
            if(getCurrentTimeMs() - t0 > TIME_TO_TRY_TO_OPEN)
			{
                errorMessageBox("Unable to open SEMotion device");
				return 1;
			}
            sleep(100); /// delay a bit then loop back and try again
		}
	}
}
///-----------------------------------------------------------------------------
/// Call this to check if a "Ready" response came back
///
/// Note:There is a possibility that other DSP threads are
/// printing to the console screen
///
/// First check if any input is available
/// if not, return instantly (SE_MOTION_OK)
/// if so, then a whole line might not be available
/// but should be in transmission and be in very soon,
/// so wait up to a short while to get in an entire line
/// if the line doesn't come in for some reason return (SE_MOTION_TIMEOUT)
/// check if the line should go to the console (esc)
/// if so print it and repeat the above as long as
/// there is still input available
/// if the line is not console data, check for "Ready",
/// if it is "Ready" send it to the console and return SE_MOTION_READY;
/// otherwise send it to the console
void MotionIOClass::charToUpper(char* mass,unsigned int length)
{
    unsigned int i;
    for(i = 0;i < length;i++,mass++)
    {
        if(*mass >= 'a' && *mass <= 'z')
        {
            *mass = *mass - 32;
        }
    }
}
SE_MOTION_STATE MotionIOClass::checkForReady()
{
	char buf[257];
	char *beg;
	char copy[257];
    int  nbytes;
    bool DetectedError(false);

    while(1)
	{
        if(_connected)
		{
            if(!numberBytesAvailToRead(&nbytes,true) && nbytes > 0)
			{
                if(readLineTimeOutRaw(buf,100) == 0)
				{
                    /// check if first char is an ESC
                    if(buf[0] == 0x1b)
                    {
                        beg = buf + 1;
                    }
					else
                    {
                        beg = buf;
                    }
					strncpy(copy,beg,255);
                    charToUpper(&copy[0],257); /// strupr(copy);
                    /// check for "Error"
                    if(strstr(copy,"ERROR") != nullptr)
                    { /// error, send to console
                        DetectedError = true;
					}
                    /// check for "Ready"
                    if(strcmp(beg,"Ready\r\n"))
                    { /// no, strange send to console
                        logToConsole(beg);
					}
					else
                    { /// yes! send to console and return Ready
                        logToConsole(beg);
                        if(DetectedError)
                        {
                            return SE_MOTION_ERROR; /// a line contained the word "error"
                        }
						else
                        {
                            return SE_MOTION_READY;
                        }
					}
				}
				else
				{
                    return SE_MOTION_TIMEOUT; /// not a whole line for some reason
				}
			}
			else
			{
                return SE_MOTION_OK; /// no data, return OK, not Ready
			}
		}
		else
		{
            return SE_MOTION_ERROR; /// not connected return error
		}
	}
}
///-----------------------------------------------------------------------------
/// Reads a line and if it gets a line with no timeout
/// check if it is a user message (indicated by esc char
/// at the beginning.  If so print it to the Console
/// repeat until we get a normal line to be returned
/// to the caller
int MotionIOClass::readLineTimeOut(char *buf,int TimeOutms)
{
	int result;
	bool Done;

	do 
	{
        Done = true;
        result = readLineTimeOutRaw(buf,TimeOutms);
        if(result==0)
		{
            /// check if first char is an ESC
            if(buf[0] == 0x1b)
			{
                /// it is!
                /// if next character is 1-7
                /// handle it as a write file command
                if(buf[1] >= 1 && buf[1] <= 7)
				{
                    /// it is a disk command
                    handleDiskIO(buf+1);
				}
				else
				{
                    /// Send to console
                    logToConsole(buf+1);
				}
                Done = false;
			}
		}
	}
    while(!Done);
	return result;
}

///-----------------------------------------------------------------------------
/// special prefix tells DSP not to echo
/// return 0 if successful
/// retirn 1 if failed
int MotionIOClass::writeLine(const char *s)
{
	char s2[MAX_LINE+1]="\x1b\x01";
	strcat(s2,s);
    return writeLineWithEcho(s2);
}
///-----------------------------------------------------------------------------
/// return 0 if successful
/// retirn 1 if failed
int MotionIOClass::writeLineWithEcho(const char *s)
{
	FT_STATUS ftStatus;
	DWORD BytesWritten;
    DWORD length;
	char s2[MAX_LINE+1];   

    if(makeSureConnected())
    {
        return 1;
    }
    strcpy(s2,s);
	strcat(s2,"\r"); // Add CR

    length = static_cast<DWORD>(strlen(s2));

    _mutex->lock();
    ftStatus = FT_Write(_ftHandle, s2, length, &BytesWritten);
    _mutex->unlock();
    if(ftStatus != FT_OK)
    {
        return(1);
    }
	return 0;
}
///-----------------------------------------------------------------------------
/// return 0 if successful
/// retirn 1 if failed
int MotionIOClass::setLatency(UCHAR LatencyTimer)
{
	FT_STATUS ftStatus;
	unsigned char c;

    ftStatus = FT_GetLatencyTimer(_ftHandle,&c);
    if(ftStatus != FT_OK)
    {
        errorMessageBox("Unable to get USB Latency Timer Value");
        return 1;
    }
    ftStatus = FT_SetLatencyTimer(_ftHandle,LatencyTimer);
    if(ftStatus == FT_OK)
	{ 
        ftStatus = FT_GetLatencyTimer (_ftHandle, &c);
        if(ftStatus != FT_OK)
        {
            errorMessageBox("Unable to get USB Latency Timer Value");
            return 1;
        }
        /// LatencyTimer set
        ftStatus = FT_SetChars(_ftHandle,'\n',1,0,0);
        if(ftStatus == FT_OK)
        { /// Event set
			return 0;
		}
		else 
        { /// FT_SetLatencyTimer FAILED!
            errorMessageBox("Unable to set USB Event Character");
			return 1;
		}
	}
	else 
	{ 
		// FT_SetLatencyTimer FAILED!
        errorMessageBox("Unable to set USB Latency timer");
		return 1;
	}
}
///-----------------------------------------------------------------------------
/// return 1 if fail
/// return 0 if successful
int MotionIOClass::flushInputBuffer()
{
    FT_STATUS ftStatus;
    DWORD RxBytes;
    DWORD TxBytes;
    DWORD BytesReceived;
    DWORD BytesWritten;
    DWORD EventDWord;
    char s[10];
    char RxBuffer[500];

    ftStatus = FT_Purge(_ftHandle,FT_PURGE_RX|FT_PURGE_TX);
    if(ftStatus != FT_OK)
    {
        return 1;
    }
    /// discard any data in the read queue in the driver
    DWORD t0 = getCurrentTimeMs();
    do
    {
        ftStatus = FT_GetStatus(_ftHandle,&RxBytes,&TxBytes,&EventDWord);
        if(ftStatus != FT_OK)
        {
            return 1;
        }
        if(RxBytes > 0)
        {
            if(RxBytes > 400)
            {
                RxBytes = 400;
            }
            ftStatus = FT_Read(_ftHandle,RxBuffer,RxBytes,&BytesReceived);
        }
    }
    while(RxBytes > 0 && getCurrentTimeMs() - t0 < CONNECT_TIMEOUT);
    if(_sendAbortOnConnect)
    {
        /// send flush command to DSP
        s[0] = ABORT_CHAR;
        s[1] = 0;
        ftStatus = FT_Write(_ftHandle,s,1,&BytesWritten);
        if(ftStatus != FT_OK)
        {
            return 1;
        }
        if(BytesWritten != 1)
        {
            return 1;
        }
        /// wait and be sure chars are transmitted
        t0 = getCurrentTimeMs();
        do
        {
            ftStatus = FT_GetStatus(_ftHandle,&RxBytes,&TxBytes,&EventDWord);
            if(ftStatus != FT_OK)
            {
                return 1;
            }
        }
        while(TxBytes != 0 && getCurrentTimeMs() - t0 < CONNECT_TIMEOUT);
        if(TxBytes != 0)
        {
            return 1;
        }
        /// wait for a fixed time for the abort acknowledge
        /// to come back which is exactly 3 characters ESC C \r
        t0 = getCurrentTimeMs();
        do
        {
            ftStatus = FT_GetStatus(_ftHandle,&RxBytes,&TxBytes,&EventDWord);
            if(ftStatus != FT_OK)
            {
               return 1;
            }
        }
        while(RxBytes < 3 && getCurrentTimeMs() - t0 < CONNECT_TIMEOUT);
        if(RxBytes == 0)
        {
            /// KMotion seems to be present but not responding
            /// after several attemps flag as non responsive and
            /// stop trying
            _nonRespondingCount++;
            if (_nonRespondingCount == CONNECT_TRIES)
            {
                errorMessageBox("SEMotion present but not responding\r\r"
                                "Correct problem and restart application");
            }
            return 1;
        }
        if(RxBytes != 3)
        {
            return 1;
        }
        _nonRespondingCount = 0;
        /// read the 3 bytes
        ftStatus = FT_Read(_ftHandle,RxBuffer,RxBytes,&BytesReceived);
        if(ftStatus != FT_OK)
        {
            return 1;
        }
        if(BytesReceived != 3)
        {
            return 1;
        }
        if(RxBuffer[0] != '\x1b')
        {
            return 1;
        }
        if(RxBuffer[1] != 'C')
        {
            return 1;
        }
        if(RxBuffer[2] != '\r')
        {
            return 1;
        }
    }
    /// verify there are no transmit or receive characters
    ftStatus = FT_GetStatus(_ftHandle,&RxBytes,&TxBytes,&EventDWord);
    if(ftStatus != FT_OK)
    {
        return 1;
    }
    if(RxBytes != 0)
    {
        return 1;
    }
    if(TxBytes != 0)
    {
        return 1;
    }
    /// OK looks like we are in sync
    return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::logToConsole(char *s)
{
    if(ConsoleHandler)
    {
        ConsoleHandler(s);
    }
    return 0;
}
///-----------------------------------------------------------------------------
/// return 0 if successful
/// retirn 1 if failed
int MotionIOClass::handleDiskIO(char *s)
{
    static FILE *f  = nullptr;
    static FILE *fr = nullptr;

    int len = static_cast<int>(strlen(s));

    if(len > 2)
    {
        s[len-2] = 0; /// strip off the CR LF
    }
    /// esc code 1 = fopen wt
    /// esc code 2 = fprintf
    /// esc code 3 = fclose write
    /// esc code 4 = fopen rt
    /// esc code 5 = read next
    /// esc code 6 = fclose read
    /// esc code 7 = fopen wt
    if(s[0] == 1)
    { /// file open
        if(f)
        {
            fclose(f);
        }
        f = fopen(s+1,"wt");
    }
    else if(s[0] == 2)
    { /// file write
        if(len > 2)
        {
            s[len - 2] = '\n';
            s[len - 1] = 0;
            if(f)
            {
                fputs(s+1,f);
            }
        }
    }
    else if(s[0] == 3)
    { /// file close
        if(f)
        {
            fclose(f);
        }
        f = nullptr;
    }
    else if(s[0] == 4)
    { /// file open read
        if(fr)
        {
            fclose(fr);
        }
        fr = fopen(s+1,"rt");
        if(fr)
        {
            return readSendNextLine(fr);
        }
        else
        {
            return writeLine("ReadDiskData 2 0");  /// flag as file open error
        }
    }
    else if(s[0] == 5)
    { /// file read
        readSendNextLine(fr);
    }
    else if(s[0] == 6)
    { /// file close
        if(fr)
        {
            fclose(fr);
        }
        fr = nullptr;
    }
    else if(s[0] == 7)
    { /// file open append text mode
        if(f)
        {
            fclose(f);
        }
        f = fopen(s + 1,"at");
    }
    return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::makeSureConnected()
{
    if(!_connected)
    {
        if(connect())
        {
            return 1;
        }
    }
    return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::numberBytesAvailToRead(int *navail,bool ShowMessage)
{
    FT_STATUS ftStatus;
    DWORD EventDWord;
    DWORD RxBytes;
    DWORD TxBytes;

    *navail = static_cast<int>(strlen(_saveChars)); /// take into account any already read in
    ftStatus = FT_GetStatus(_ftHandle,&RxBytes,&TxBytes,&EventDWord);
    if(ftStatus != FT_OK)
    {
        if(ShowMessage)
        {
            failed();
        }
        else
        {
            _connected = false;
        }
        return 1;
    }
    else
    {
        *navail += static_cast<int>(RxBytes);
        return 0;
    }
}
///-----------------------------------------------------------------------------
int MotionIOClass::readLineTimeOutRaw(char *buf, int TimeOutms)
{
    BOOL  Done(FALSE);
    int   TotalBytes;
    int   result;
    DWORD NBytesRead;
    char *p;
    char  ReadBuffer[MAX_LINE];
    int   i;
    int   freespace;
    bool  FirstTime=true;
    static bool ErrorDisplayed=false;


    if(ErrorDisplayed)
    {
        return 1;
    }

    strcpy(buf,_saveChars);
    TotalBytes = static_cast<int>(strlen(buf));
    _saveChars[0] = 0; /// remember we used them

    DWORD t0 = getCurrentTimeMs();
    while(!Done)
    {
        if(FirstTime)
        {
            FirstTime = false;
        }
        else
        {
            sleep(1);
        }
        for(i = 0;i < 200;i++)
        {
            ReadBuffer[i] = 0;
        }
        freespace = MAX_LINE - TotalBytes - 1;
        if(freespace > 0)
        {
            result = readBytesAvailable(ReadBuffer,freespace,&NBytesRead,0);
            if(result)
            {
                return result;
            }
            if(NBytesRead > 0)
            {
                for(i=0;i < static_cast<int>(NBytesRead); i++)
                {
                    if(i+TotalBytes > MAX_LINE)
                    {
                        errorMessageBox("SEMotion Received Line too long");
                        return 1;
                    }
                    buf[i+TotalBytes] = ReadBuffer[i];
                }
            }
            TotalBytes += NBytesRead;
            /// Successfully received data.  Zero terminate string.
            buf[TotalBytes] = 0;
        }
        /// check if we got a terminating character  \n
        p = strchr(buf,'\n');
        if(p)
        {
            /// check if we read extra characters
            if ((p-buf) < static_cast<int>(strlen(buf)-1))
            {
                /// yes, save them for next time
                strcpy(_saveChars,p+1);
                /// now zero terminate first string
                /// after the \r
                *(p+1) = 0;
            }
            Done = TRUE;
        }
        if(!NO_SEMOTION_TIMEOUT && !Done && static_cast<int>(getCurrentTimeMs()-t0) > TimeOutms)
        {
            return 2; /// return with timeout indication
        }

    }
    _failMessageAlreadyShown = false; /// if we successfully received something show any subsequent error
    return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::readBytesAvailable(char  *RxBuffer,
                                      int    maxbytes,
                                      DWORD *BytesReceived,
                                      int    timeout_ms)
{
    FT_STATUS ftStatus;
    DWORD EventDWord;
    DWORD RxBytes;
    DWORD TxBytes;

    ftStatus = FT_GetStatus(_ftHandle,&RxBytes,&TxBytes,&EventDWord);

    if(ftStatus != FT_OK)
    {
        failed();
        return 1;
    }
    if(static_cast<int>(RxBytes) > maxbytes)
    {
        RxBytes = static_cast<DWORD>(maxbytes - 1); /// leave room for null
    }

    RxBuffer[0] = 0; /// set buf empty initially
    *BytesReceived = 0;
    if(RxBytes > 0)
    {
        ftStatus = FT_Read(_ftHandle,RxBuffer,RxBytes,BytesReceived);
        if(ftStatus == FT_OK)
        {
            RxBuffer[*BytesReceived] = 0; /// null terminate
        }
        else
        {
            failed();
            return 1;
        }
    }
    return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::readSendNextLine(FILE *fr)
{
    char s[1024];
    char w[256];
    char *src;
    char *dst;
    int  n;
    int  i;
    int  k;

    if(fr)
    {
        if(feof(fr))
        { /// eof?
            return writeLine("ReadDiskData 3 0");  ///signal eof
        }
        else
        {
            if(fgets(s,1024,fr)==nullptr)
            {
                return writeLine("ReadDiskData 3 0");  /// signal eof
            }
            /// remove any CR LF characters
            for(src = dst = s; *src != '\0'; src++)
            {
                *dst = *src;
                if (*dst != '\n' && *dst != '\r') dst++;
            }
            *dst = '\0';

            n = static_cast<int>(strlen(s));
            sprintf(w,"ReadDiskData 1 %d",n);
            if (writeLine(w))
            {
                return 1;
            }
            /// reset bytes/line
            for(i = 0,k = 0;i < n;i++)
            {
                sprintf(w+3*k,"%02X ",s[i]); /// append hex code
                k++;
                if(k == 80) /// full line?
                {
                    w[3*k-1] = 0; /// null terminate
                    if(writeLine(w)) ///send the line
                    {
                        return 1;
                    }
                    k = 0;
                }
            }
            if(k > 0) /// any partial line remaining?
            {
                w[3*k-1] = 0;  /// null terminate
                if(writeLine(w))  /// send the line
                {
                    return 1;
                }
            }
        }
    }
    else
    {
        return writeLine("ReadDiskData 2 0"); /// signal error
    }
    return 0;
}
///-----------------------------------------------------------------------------
/// return 0 if successful
/// retirn 1 if failed
int MotionIOClass::writeLineReadLine(const char *send,char *response)
{
    _mutex->lock();
    if(writeLine(send))
	{
        _mutex->unlock();
		return 1;
	}
    if(readLineTimeOut(response,3000))
	{
        _mutex->unlock();
		return 1;
	}
    response[strlen(response)-2] = 0; /// remove the /r /n
    _mutex->unlock();
	return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::failed()
{
    _mutex->lock();
    _connected = false;
    FT_Close(_ftHandle);
    if(!_failMessageAlreadyShown)
    {
        errorMessageBox("Read Failed - Auto Disconnect");
    }
    _failMessageAlreadyShown = true;
    _mutex->unlock();
	return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::disconnect()
{
    _mutex->lock();
    _connected = false;
    FT_Close(_ftHandle);
    _mutex->unlock();
	return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::firmwareVersion()
{
    return _firmwareVersion;
}
///-----------------------------------------------------------------------------
/// if connected, returns the USB location code associated with this object
QString MotionIOClass::usbLocation()
{
    if(_connected)
    {
        return _serialNumber;
    }
    else
    {
        return(nullptr);
    }
}
///-----------------------------------------------------------------------------
/// returns 0 SE_MOTION_LOCKED (and _token is locked) if KMotion is available for use
/// returns 1 SE_MOTION_IN_USE if already in use
/// returns 2 SE_MOTION_NOT_CONNECTED if not able to connect
/// stronger than a _mutex lock because it protects against the same thread
SE_MOTION_LOCK_STATE MotionIOClass::motionLock(char *CallerID)
{
    if(!_mutex->tryLock(3000))
    {
        return SE_MOTION_NOT_CONNECTED;
    }
    if(!_connected)
	{
        /// try to connect
        if(!requestedDeviceAvail(nullptr))
        {
            _nonRespondingCount = 0;
            _mutex->unlock();  /// no such device available
            return SE_MOTION_NOT_CONNECTED;
        }
        if(connect())
		{
            _mutex->unlock();  /// couldn't connect
            return SE_MOTION_NOT_CONNECTED;
		}
	}
    if(_token == 0)
	{
        _token++;
        if(CallerID == nullptr)
        {
            _lastCallerID = "";
        }
		else
        {
            _lastCallerID = CallerID;
        }
        _mutex->unlock();
        return(SE_MOTION_LOCKED);
	}
	else
	{
        _mutex->unlock();
        return(SE_MOTION_IN_USE);
	}
}
///-----------------------------------------------------------------------------
/// Special routine to connect to KMotion board
/// without aborting out of the recovery Bootloader
SE_MOTION_LOCK_STATE MotionIOClass::motionLockRecovery()
{
    SE_MOTION_LOCK_STATE result;
    char CallerID[] = "KMotionLockRecovery";
    _sendAbortOnConnect = false;
    result = motionLock(&CallerID[0]);
    _sendAbortOnConnect = true;
    return result;
}
///-----------------------------------------------------------------------------
void MotionIOClass::releaseToken()
{
    _mutex->lock();
    _lastCallerID = "";
    _token--;
    if(_token < 0)
    {
         _token = 0;
    }
    _mutex->unlock();
}
///-----------------------------------------------------------------------------
/// Print any data to the console
int MotionIOClass::serviceConsole()
{
    int nbytes;
    int timeout;
	char b[MAX_LINE];
    char CallerID[] = "Service Console";
    /// quick check if it is available
    if (motionLock(&CallerID[0]) == SE_MOTION_LOCKED)
	{
        if(!numberBytesAvailToRead(&nbytes,false) && nbytes > 0)
		{
            /// some data in the buffer
            timeout = readLineTimeOutRaw(b,100);
            if(!timeout)
			{
                if (b[0] == 0x1b) /// skip over esc if there is one
                {
                    logToConsole(b+1);
                }
				else
                {
                    logToConsole(b);
                }
			}
		}
        releaseToken();
	}
	return 0;
}

///-----------------------------------------------------------------------------
int MotionIOClass::setConsoleCallback(SERVER_CONSOLE_HANDLER *ch)
{
	ConsoleHandler = ch;
	return 0;
}
///-----------------------------------------------------------------------------
