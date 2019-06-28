///-----------------------------------------------------------------------------
#include <time.h>
#include <sys/time.h>
///-----------------------------------------------------------------------------
#include "common.h"
///-----------------------------------------------------------------------------
#define CONNECT_TRIES       5
#define CONNECT_TIMEOUT     1000
#define TIME_TO_TRY_TO_OPEN 3000
///-----------------------------------------------------------------------------
MotionIOClass::MotionIOClass()
{
    ConsoleHandler = nullptr;
    Mutex = new QMutex();
    m_Connected = false;
    SendAbortOnConnect = true;
    FailMessageAlreadyShown = false;
    Token = 0;
    NonRespondingCount = 0;
    m_FirmwareVersion = 2;
    USB_Loc_ID = -1;
    BoardIDAssigned = false;
}
///-----------------------------------------------------------------------------
MotionIOClass::~MotionIOClass()
{
	delete Mutex;
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
	FT_DEVICE_LIST_INFO_NODE *devInfo;
	FT_STATUS ftStatus;
	DWORD numDevs;  
	int i;

    Mutex->lock();

    /// create the device information list
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if (ftStatus == FT_OK) 
	{ 
		if (numDevs > 0) 
		{ 
            /// allocate storage for list based on numDevs
            ///devInfo = (FT_DEVICE_LIST_INFO_NODE*) malloc(sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs);
            devInfo = new FT_DEVICE_LIST_INFO_NODE[sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs];
            /// get the device information list
			ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
            for(i=0;i < static_cast<int>(numDevs);i++)
			{
                if(
                   strstr(devInfo[i].Description,"KFLOP") == nullptr &&
                   strstr(devInfo[i].Description,"KMotion") == nullptr &&
                   strstr(devInfo[i].Description,"Dynomotion") == nullptr
                   )
				{
                    /// remove it from the list
                    for(int k = i + 1;k < static_cast<int>(numDevs);k++)
						devInfo[k-1] = devInfo[k];  // shift up
                    numDevs--;
                    i--; /// redo this slot since it was deleted and things shifted up
				}
			}
            /// if USB Location is undefined select the first from
            /// the list that is not already taken
            if (!BoardIDAssigned)
            {
                for(i = 0;i < static_cast<int>(numDevs) && !BoardIDAssigned;i++)
                {
                    int k;
                    /// make sure nobody is already using this one
                    for(k = 0;k < MAX_BOARDS;k++)
                    {
                        if(devInfo[i].LocId == static_cast<DWORD>(USB_Loc_ID))
                            break;
                    }
                    if(k == MAX_BOARDS)
                    {
                        BoardIDAssigned = true;
                        USB_Loc_ID = static_cast<int>(devInfo[i].LocId);  // assign it
                    }
                }
                if (!BoardIDAssigned)
                {
                    Mutex->unlock();
                    if(Reason) *Reason = "No SEMotion devices available";
                    return false;
                }
            }
            /// user wants a specific usb location
            /// so see if it is available
            for(i = 0;i < static_cast<int>(numDevs);i++)
			{
                if(devInfo[i].LocId == static_cast<DWORD>(USB_Loc_ID))
					break;
			}
            if(i == static_cast<int>(numDevs))
            { /// not find the devices
                Mutex->unlock();
                if(Reason)
                {
                    *Reason = QString("SEMotion not found on USB Location %08X\r\r Unable to open device")
                                .arg(USB_Loc_ID);
                }
				return false;
			}
			delete (devInfo);
		}
		else
		{
            Mutex->unlock();
            if(Reason) *Reason ="No SEMotion devices available";
			return false;
		}
	}
    Mutex->unlock();
	return true;
}
///-----------------------------------------------------------------------------
int MotionIOClass::connect()
{
    QString reason;
	FT_STATUS ftStatus;

    if(NonRespondingCount == CONNECT_TRIES)
        return 1;

    m_SaveChars[0] = 0; /// start anew
    Mutex->lock();

    if(!requestedDeviceAvail(&reason))
	{
        errorMessageBox(reason.toStdString().c_str());
        Mutex->unlock();
		return 1;
	}
    /// FT_ListDevices OK, number of devices connected is in numDevs
    /// usually during boot the board comes and goes, since it appeared
    /// to be there, try for a while to open it
    DWORD t0 = getCurrentTimeMs();
    while(1)
	{
        ///ftStatus = FT_OpenEx((void *)USB_Loc_ID,FT_OPEN_BY_LOCATION,&ftHandle);
        ftStatus = FT_OpenEx(static_cast<void*>(&USB_Loc_ID),FT_OPEN_BY_LOCATION,&ftHandle);
        if(ftStatus == FT_OK)
        { /// FT_Open OK, use ftHandle to access device
            if(setLatency(2))
			{
                Mutex->unlock();
				return 1;
			}
            if(flushInputBuffer())
			{
				FT_Close(ftHandle);
                Mutex->unlock();
				return 1;
			}
            m_Connected = true;  // All set
            Mutex->unlock();
			return 0;
		}
		else 
        { /// FT_Open failed
            if(getCurrentTimeMs() - t0 > TIME_TO_TRY_TO_OPEN)
			{
                errorMessageBox("Unable to open SEMotion device");
                Mutex->unlock();
				return 1;
			}
            sleep(100); /// delay a bit then loop back and try again
		}
	}
}
///-----------------------------------------------------------------------------
int MotionIOClass::numberBytesAvailToRead(int *navail,bool ShowMessage)
{
	FT_STATUS ftStatus;
	DWORD EventDWord;
	DWORD RxBytes;
	DWORD TxBytes;

    *navail = static_cast<int>(strlen(m_SaveChars)); /// take into account any already read in
	
    Mutex->lock();
	ftStatus = FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
    if(ftStatus != FT_OK)
	{
        if(ShowMessage)
        {
            failed();
        }
		else
        {
			m_Connected=false;
        }
        Mutex->unlock();
		return 1;
	}
	else
	{
        *navail += static_cast<int>(RxBytes);
        Mutex->unlock();
		return 0;
	}
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

    Mutex->lock();
	ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);

    if(ftStatus != FT_OK)
	{
        failed();
        Mutex->unlock();
		return 1;
	}
    if(static_cast<int>(RxBytes) > maxbytes)
    {
        RxBytes = static_cast<DWORD>(maxbytes - 1); // leave room for null
    }

    RxBuffer[0] = 0; /// set buf empty initially
    *BytesReceived = 0;
    if(RxBytes > 0)
	{
		ftStatus = FT_Read(ftHandle,RxBuffer,RxBytes,BytesReceived);
        if(ftStatus == FT_OK)
		{
            RxBuffer[*BytesReceived] = 0; /// null terminate
		}
		else 
		{
            failed();
            Mutex->unlock();
			return 1;
		}
	}
    Mutex->unlock();
	return 0;
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
int MotionIOClass::checkForReady()
{
	char buf[257];
	char *beg;
	char copy[257];
    int  nbytes;
    bool DetectedError(false);

    while(1)
	{
		if (m_Connected)
		{
            if (!numberBytesAvailToRead(&nbytes,true) && nbytes > 0)
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
///                    strupr(copy);
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
    Mutex->lock();

	strcpy(buf,m_SaveChars);
    TotalBytes = static_cast<int>(strlen(buf));
    m_SaveChars[0] = 0; /// remember we used them

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
                Mutex->unlock();
				return result;
			}
            if(NBytesRead > 0)
			{
                for(i=0;i < static_cast<int>(NBytesRead); i++)
				{
                    if(i+TotalBytes > MAX_LINE)
					{
                        errorMessageBox("SEMotion Received Line too long");
                        Mutex->unlock();
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
				strcpy(m_SaveChars,p+1);
                /// now zero terminate first string
                /// after the \r
				*(p+1) = 0;
			}
            Done = TRUE;
		}
        if(!NO_SEMOTION_TIMEOUT && !Done && static_cast<int>(getCurrentTimeMs()-t0) > TimeOutms)
		{
            Mutex->unlock();
            return 2; /// return with timeout indication
		}

	}
    FailMessageAlreadyShown = false; /// if we successfully received something show any subsequent error
    Mutex->unlock();
	return 0;
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

    Mutex->lock();
	ftStatus = FT_Write(ftHandle, s2, length, &BytesWritten);
    Mutex->unlock();
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

    Mutex->lock();

    ftStatus = FT_GetLatencyTimer(ftHandle,&c);
    if(ftStatus != FT_OK)
    {
        errorMessageBox("Unable to get USB Latency Timer Value");
        Mutex->unlock();
        return 1;
    }
	ftStatus = FT_SetLatencyTimer(ftHandle,LatencyTimer);
    if(ftStatus == FT_OK)
	{ 
		ftStatus = FT_GetLatencyTimer (ftHandle, &c);
        if(ftStatus != FT_OK)
        {
            errorMessageBox("Unable to get USB Latency Timer Value");
            Mutex->unlock();
            return 1;
        }
        /// LatencyTimer set
        ftStatus = FT_SetChars(ftHandle,'\n',1,0,0);
        if(ftStatus == FT_OK)
		{ 
            /// Event set
            Mutex->unlock();
			return 0;
		}
		else 
		{ 
            /// FT_SetLatencyTimer FAILED!
            errorMessageBox("Unable to set USB Event Character");
            Mutex->unlock();
			return 1;
		}
	}
	else 
	{ 
		// FT_SetLatencyTimer FAILED!
        errorMessageBox("Unable to set USB Latency timer");
        Mutex->unlock();
		return 1;
	}
}
///-----------------------------------------------------------------------------
/// return 0 if successful
/// retirn 1 if failed
int MotionIOClass::writeLineReadLine(const char *send,char *response)
{
    Mutex->lock();
    if(writeLine(send))
	{
        Mutex->unlock();
		return 1;
	}
    if(readLineTimeOut(response,3000))
	{
        Mutex->unlock();
		return 1;
	}
    response[strlen(response)-2] = 0; /// remove the /r /n
    Mutex->unlock();
	return 0;
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

	ftStatus = FT_Purge(ftHandle,FT_PURGE_RX|FT_PURGE_TX);
    if(ftStatus != FT_OK)
    {
        return 1;
    }
    /// discard any data in the read queue in the driver
    DWORD t0 = getCurrentTimeMs();
	do
	{
        ftStatus = FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
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
			ftStatus = FT_Read(ftHandle,RxBuffer,RxBytes,&BytesReceived);
		}
	}
    while(RxBytes > 0 && getCurrentTimeMs() - t0 < CONNECT_TIMEOUT);
    if(SendAbortOnConnect)
	{
        /// send flush command to DSP
        s[0] = ABORT_CHAR;
        s[1] = 0;
        ftStatus = FT_Write(ftHandle,s,1,&BytesWritten);
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
            ftStatus = FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
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
            ftStatus = FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
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
			NonRespondingCount++;
			if (NonRespondingCount == CONNECT_TRIES)
			{
                errorMessageBox("KMotion present but not responding\r\r"
								"Correct problem and restart application");
			}
			return 1;
		}
        if(RxBytes != 3)
        {
            return 1;
        }
        NonRespondingCount = 0;
        /// read the 3 bytes
        ftStatus = FT_Read(ftHandle,RxBuffer,RxBytes,&BytesReceived);
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
    ftStatus = FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
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
int MotionIOClass::failed()
{
    Mutex->lock();
	m_Connected=false;
	
	FT_Close(ftHandle);
    if(!FailMessageAlreadyShown)
    {
        errorMessageBox("Read Failed - Auto Disconnect");
    }
    FailMessageAlreadyShown = true;
    Mutex->unlock();
	return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::disconnect()
{
    Mutex->lock();
    m_Connected = false;
	FT_Close(ftHandle);
    Mutex->unlock();
	return 0;
}
///-----------------------------------------------------------------------------
int MotionIOClass::firmwareVersion()
{
	return m_FirmwareVersion;
}
///-----------------------------------------------------------------------------
/// if connected, returns the USB location code associated with this object
int MotionIOClass::usbLocation()
{
    if(m_Connected)
    {
        return USB_Loc_ID;
    }
    else
    {
        return(-1);
    }
}
///-----------------------------------------------------------------------------
/// returns 0 KMOTION_LOCKED (and token is locked) if KMotion is available for use
/// returns 1 KMOTION_IN_USE if already in use
/// returns 2 KMOTION_NOT_CONNECTED if not able to connect
/// stronger than a Mutex lock because it protects against the same thread
int MotionIOClass::motionLock(char *CallerID)
{
	int result;
    ///int board = this - SEMotionIO;
    if(!Mutex->tryLock(3000))
    {
        return SE_MOTION_NOT_CONNECTED;
    }
    if(!m_Connected)
	{
        /// try to connect
        if(!requestedDeviceAvail(nullptr))
		{
			NonRespondingCount=0;
            Mutex->unlock();  /// no such device available
            return SE_MOTION_NOT_CONNECTED;
		}
        if(connect())
		{
            Mutex->unlock();  /// couldn't connect
            return SE_MOTION_NOT_CONNECTED;
		}
	}
    if(Token == 0)
	{
		Token++;
        if(CallerID==nullptr)
        {
            m_LastCallerID = "";
        }
		else
        {
            m_LastCallerID = CallerID;
        }
        result = SE_MOTION_LOCKED;
	}
	else
	{
        result = SE_MOTION_IN_USE;
	}
    Mutex->unlock();
	return result;
}
///-----------------------------------------------------------------------------
/// Special routine to connect to KMotion board
/// without aborting out of the recovery Bootloader
int MotionIOClass::motionLockRecovery()
{
    int  result;
    char CallerID[] = "KMotionLockRecovery";
	SendAbortOnConnect=false;
    result = motionLock(&CallerID[0]);
	SendAbortOnConnect=true;
    return result;
}
///-----------------------------------------------------------------------------
void MotionIOClass::releaseToken()
{
    Mutex->lock();
	m_LastCallerID = "";
	Token--;
    if (Token < 0) Token=0; /// ADDED THIS LINE TO ENFORCE Token >= 0
    Mutex->unlock();
}
///-----------------------------------------------------------------------------
int MotionIOClass::logToConsole(char *s)
{
    if(ConsoleHandler)
    {
        ConsoleHandler(0,s);
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
            Mutex->lock();
			sprintf(w,"ReadDiskData 1 %d",n);
            if (writeLine(w))
			{
                Mutex->unlock();
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
                        Mutex->unlock();
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
                    Mutex->unlock();
					return 1;
				}
			}
            Mutex->unlock();
		}
	}
	else
	{
        return writeLine("ReadDiskData 2 0"); /// signal error
	}
	return 0;
}
///-----------------------------------------------------------------------------
/// Print any data to the console
int MotionIOClass::serviceConsole()
{
	int nbytes, timeout;
	char b[MAX_LINE];
    char CallerID[] = "Service Console";
    /// quick check if it is available
    if (motionLock(&CallerID[0]) == SE_MOTION_LOCKED)
	{
        if(!numberBytesAvailToRead(&nbytes,false) && nbytes>0)
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
int MotionIOClass::makeSureConnected()
{
    if(!m_Connected)
	{
        if(connect())
		{
			return 1;
		}
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
// save the error message to be piped back to caller
int MotionIOClass::errorMessageBox(const char *s)
{
	ErrMsg = s;
	return 0;
}
///-----------------------------------------------------------------------------
