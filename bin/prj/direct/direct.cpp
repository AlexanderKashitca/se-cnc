///-----------------------------------------------------------------------------
#include "direct.h"
///-----------------------------------------------------------------------------
MotionDirectClass::MotionDirectClass()
{
}
///-----------------------------------------------------------------------------
// Maps a specified Board Identifiers to a MotionIO Index (or object)
//
// A board identifier may be any of the following:
//
//    #1  a USB location ID such as 0x00000321
//    #2  0 .. -15 which maps to the first through the 15th
//        currently available device (which has not been opened
//        by location)
//
// We maintain within each MotionIO object whether it has been assigned
// and if so, which Board Identifier and which USB location it is assigned to
//
// If a new Board Identifier is encountered the first available MotionIO
// object is assigned to it
int MotionDirectClass::mapBoardToIndex(int BoardID)
{
    int i;
    if(BoardID <= 0 && BoardID >= -15)
    { /// simply use the BoardID as the object
        return -BoardID;
    }
    /// if requested BoardID is specified as a USB location
    /// first scan all objects to see if one is already connected
    /// to that USB location
    for(i = 0;i < MAX_BOARDS;i++)
    {
        if(MotionIO.BoardIDAssigned &&
           MotionIO.m_Connected &&
           MotionIO.USB_Loc_ID == BoardID
        )
        { /// found previously assigned matching Location use it
            return i;
        }
    }
    for(i = 0;i < MAX_BOARDS;i++)
    {
        if(MotionIO.BoardIDAssigned)
        {
            if(MotionIO.USB_Loc_ID == BoardID)
            { /// found previously assigned matching object, return it
                return i;
            }
        }
    }
    /// BoardID never before encountered
    MotionIO.Mutex->lock(); /// better lock while assigning objects
    /// find an available object to handle it
    for(i = 0;i < MAX_BOARDS;i++)
    {
        if(!MotionIO.BoardIDAssigned)
        {
            break;
        }
    }
    if(i == MAX_BOARDS)
    {
///		AfxMessageBox("Fatal Error: Too Many Board IDs used",MB_ICONSTOP|MB_OK);
        MotionIO.Mutex->unlock();
        exit(1);
    }
    MotionIO.BoardIDAssigned = true;
    MotionIO.USB_Loc_ID = BoardID; /// assign the ID
    MotionIO.Mutex->unlock();
    return i;
}
///-----------------------------------------------------------------------------
int MotionDirectClass::listLocations(int *nlocations, int *list)
{
    FT_DEVICE_LIST_INFO_NODE *devInfo;
    FT_STATUS ftStatus;
    DWORD numDevs;
    int i;

    list[0] = -1;
    *nlocations = 0;
    /// create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if(ftStatus == FT_OK)
    {
        if(numDevs > 0)
        {
            /// allocate storage for list based on numDevs
            ///devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs); // get the device information list
            devInfo = new FT_DEVICE_LIST_INFO_NODE[sizeof(FT_DEVICE_LIST_INFO_NODE) * numDevs];
            /// get the device information list
            ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
            /// go through the list and copy Dynomotion USB IDs to User's list
            for(i = 0;i < static_cast<int>(numDevs);i++)
            {
                //if
                //(
                //    strstr(devInfo[i].Description,"KFLOP")!= nullptr ||
                //    strstr(devInfo[i].Description,"KMotion")!= nullptr ||
                //    strstr(devInfo[i].Description,"Dynomotion")!= nullptr
                //)
                //{
                    list[(*nlocations)++] = static_cast<int>(devInfo[i].LocId);
                //}
            }
            delete (devInfo);
        }
    }
    else
    { /// Create List Failed
        return 1;
    }
    return 0;
}
///-----------------------------------------------------------------------------
int MotionDirectClass::writeLineReadLine(const char *s, char *response)
{
    return MotionIO.writeLineReadLine(s, response);
}
///-----------------------------------------------------------------------------
int MotionDirectClass::writeLine(const char *s)
{
    return MotionIO.writeLine(s);
}
///-----------------------------------------------------------------------------
int MotionDirectClass::writeLineWithEcho(const char *s)
{
    return MotionIO.writeLineWithEcho(s);
}
///-----------------------------------------------------------------------------
int MotionDirectClass::readLineTimeOut(char *buf, int TimeOutms)
{
    return MotionIO.readLineTimeOut(buf,TimeOutms);
}
///-----------------------------------------------------------------------------
int MotionDirectClass::failed()
{
    return MotionIO.failed();
}
///-----------------------------------------------------------------------------
int MotionDirectClass::disconnect()
{
    return MotionIO.disconnect();
}
///-----------------------------------------------------------------------------
int MotionDirectClass::firmwareVersion()
{
    return MotionIO.firmwareVersion();
}
///-----------------------------------------------------------------------------
int MotionDirectClass::checkForReady()
{
    return MotionIO.checkForReady();
}
///-----------------------------------------------------------------------------
int MotionDirectClass::motionLock(char *CallerID)
{
    return MotionIO.motionLock(CallerID);
}
///-----------------------------------------------------------------------------
int MotionDirectClass::usbLocation()
{
    return MotionIO.usbLocation();
}
///-----------------------------------------------------------------------------
int MotionDirectClass::motionLockRecovery()
{
    return MotionIO.motionLockRecovery();
}
///-----------------------------------------------------------------------------
void MotionDirectClass::releaseToken()
{
    MotionIO.releaseToken();
}
///-----------------------------------------------------------------------------
int  MotionDirectClass::serviceConsole()
{
    return MotionIO.serviceConsole();
}
///-----------------------------------------------------------------------------
int MotionDirectClass::setConsoleCallback(SERVER_CONSOLE_HANDLER *ch)
{
    MotionIO.setConsoleCallback(ch);
    return 0;
}
///-----------------------------------------------------------------------------
int MotionDirectClass::nInstances()
{
    return(1);
}
///-----------------------------------------------------------------------------
const char * MotionDirectClass::getErrMsg()
{
    return MotionIO.ErrMsg.toStdString().c_str();
}
///-----------------------------------------------------------------------------
void MotionDirectClass::clearErrMsg()
{
    MotionIO.ErrMsg="";
}
///-----------------------------------------------------------------------------
