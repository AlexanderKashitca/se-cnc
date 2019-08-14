///-----------------------------------------------------------------------------
#include "direct.h"
///-----------------------------------------------------------------------------
MOTION_DIRECT_SPACE::MotionDirectClass::MotionDirectClass()
{
    library.setFileName("libftd2xx.so");
    if(!library.load())
    {
        _ftdiLibraryLoad = false;
    }
    else
    {
        _ftdiLibraryLoad = true;
    }
}
///-----------------------------------------------------------------------------
MOTION_DIRECT_SPACE::MotionDirectClass::~MotionDirectClass()
{
    library.unload();
    _motionIO.~MotionIOClass();
}
///-----------------------------------------------------------------------------
bool MOTION_DIRECT_SPACE::MotionDirectClass::getFtdiLibraryLoad()
{
    return(_ftdiLibraryLoad);
}
///-----------------------------------------------------------------------------
QString MOTION_DIRECT_SPACE::MotionDirectClass::getLastErrorMessage()
{
    return(_motionIO.getLastErrorMessage());
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
///int MOTION_DIRECT_SPACE::MotionDirectClass::mapBoardToIndex(int BoardID)
///{
///
///    int i;
///    if(BoardID <= 0 && BoardID >= -15)
///    { /// simply use the BoardID as the object
///        return -BoardID;
///    }
///    /// if requested BoardID is specified as a USB location
///    /// first scan all objects to see if one is already connected
///    /// to that USB location
///    for(i = 0;i < MAX_BOARDS;i++)
///    {
///        if(_motionIO.BoardIDAssigned &&
///           _motionIO.m_Connected &&
///           _motionIO.USB_Loc_ID == BoardID
///        )
///        { /// found previously assigned matching Location use it
///            return i;
///        }
///    }
///    for(i = 0;i < MAX_BOARDS;i++)
///    {
///        if(_motionIO.BoardIDAssigned)
///        {
///            if(_motionIO.USB_Loc_ID == BoardID)
///            { /// found previously assigned matching object, return it
///                return i;
///            }
///        }
///    }
///    /// BoardID never before encountered
///    _motionIO.Mutex->lock(); /// better lock while assigning objects
///    /// find an available object to handle it
///    for(i = 0;i < MAX_BOARDS;i++)
///    {
///        if(!_motionIO.BoardIDAssigned)
///        {
///            break;
///        }
///    }
///    if(i == MAX_BOARDS)
///    {
///		AfxMessageBox("Fatal Error: Too Many Board IDs used",MB_ICONSTOP|MB_OK);
///     _motionIO.Mutex->unlock();
///        exit(1);
///    }
///    _motionIO.BoardIDAssigned = true;
///    _motionIO.USB_Loc_ID = BoardID; /// assign the ID
/////    _motionIO.Mutex->unlock();
///    return i;
///}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::listLocations(int *nlocations, int *list)
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
int MOTION_DIRECT_SPACE::MotionDirectClass::writeLineReadLine(const char* s,char* response)
{
    return(_motionIO.writeLineReadLine(s,response));
}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::writeLine(const char *s)
{
    return _motionIO.writeLine(s);
}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::writeLineWithEcho(const char *s)
{
    return _motionIO.writeLineWithEcho(s);
}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::readLineTimeOut(char *buf,int TimeOutms)
{
    return _motionIO.readLineTimeOut(buf,TimeOutms);
}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::failed()
{
    return _motionIO.failed();
}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::disconnect()
{
    return _motionIO.disconnect();
}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::firmwareVersion()
{
    return _motionIO.firmwareVersion();
}
///-----------------------------------------------------------------------------
SE_MOTION_STATE MOTION_DIRECT_SPACE::MotionDirectClass::checkForReady()
{
    return _motionIO.checkForReady();
}
///-----------------------------------------------------------------------------
SE_MOTION_LOCK_STATE MOTION_DIRECT_SPACE::MotionDirectClass::motionLock(char *CallerID)
{
    return _motionIO.motionLock(CallerID);
}
///-----------------------------------------------------------------------------
SE_MOTION_LOCK_STATE MOTION_DIRECT_SPACE::MotionDirectClass::motionLockRecovery()
{
    return _motionIO.motionLockRecovery();
}
///-----------------------------------------------------------------------------
QString MOTION_DIRECT_SPACE::MotionDirectClass::usbLocation()
{
    return _motionIO.usbLocation();
}
///-----------------------------------------------------------------------------
void MOTION_DIRECT_SPACE::MotionDirectClass::releaseToken()
{
    _motionIO.releaseToken();
}
///-----------------------------------------------------------------------------
int  MOTION_DIRECT_SPACE::MotionDirectClass::serviceConsole()
{
    return _motionIO.serviceConsole();
}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::setConsoleCallback(SERVER_CONSOLE_HANDLER *ch)
{
    _motionIO.setConsoleCallback(ch);
    return 0;
}
///-----------------------------------------------------------------------------
int MOTION_DIRECT_SPACE::MotionDirectClass::nInstances()
{
    return(1);
}
///-----------------------------------------------------------------------------
const char* MOTION_DIRECT_SPACE::MotionDirectClass::getErrMsg()
{
    return _motionIO._errMsg.toStdString().c_str();
}
///-----------------------------------------------------------------------------
void MOTION_DIRECT_SPACE::MotionDirectClass::clearErrMsg()
{
    _motionIO._errMsg="";
}
///-----------------------------------------------------------------------------
