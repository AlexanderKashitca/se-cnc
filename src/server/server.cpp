///-----------------------------------------------------------------------------
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>
///-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
///-----------------------------------------------------------------------------
#include "../server/server.h"
///-----------------------------------------------------------------------------
static QList <QString> ConsoleList;
static int ConsoleHandler(const char *buf);
///-----------------------------------------------------------------------------
ServerClass::~ServerClass()
{
    _motionDirect.~MotionDirectClass();
}
///-----------------------------------------------------------------------------
void ServerClass::quit()
{
    QTimer::singleShot(0, QCoreApplication::instance(), &QCoreApplication::quit);
}
///-----------------------------------------------------------------------------
QDBusVariant ServerClass::query(const QByteArray &query)
{

    QByteArray response;
    response.append("Server Response");

    if(!query.isEmpty())
    {
        for(int i = 0;i < query.length();i++)
        {
            _requestBuffer[i] = query.at(i);
        }
        _requestBytes = static_cast<unsigned int>(query.length());
        GetQueryToAnswer();

    }
    return(QDBusVariant(QByteArray(response)));
}
///-----------------------------------------------------------------------------
bool ServerClass::GetQueryToAnswer()
{
    int code;
    ///int board;
    int BoardID;
    int result(0);
    ///int List[256];
    int TimeOutms;
    ///int nLocations;

    memcpy(&code,&_replyBuffer[0],4);

    if(code != ENUM_ListLocations)
    { /// all commands require a board to be mapped, except this command
        memcpy(&BoardID,&_replyBuffer[4],4);
        ///board=_motionDirect.MapBoardToIndex(BoardID);
    }

    _replyBuffer[0] = DEST_NORMAL;
    switch(code)
    {
        case ENUM_WriteLineReadLine :
            /// Send Code, board, string -- Get Dest byte, Result (int) and string
            result = _motionDirect.writeLineReadLine(&_requestBuffer[8],&_replyBuffer[5]);
            memcpy(&_replyBuffer[1],&result,4);
            /// Dest byte, Result int, string, null char
            _replyBytes = 6;
            _replyBytes += static_cast<unsigned int>(strlen(&_replyBuffer[5]));
            break;
        case ENUM_WriteLine :
            result = _motionDirect.writeLine(&_requestBuffer[8]);
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_WriteLineWithEcho :
            result = _motionDirect.writeLineWithEcho(&_requestBuffer[8]);
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_ReadLineTimeOut :
            /// Send Code, board, timeout -- Dest byte, Get Result (int), and string
            memcpy(&TimeOutms,&_requestBuffer[8],4);
            result = _motionDirect.readLineTimeOut(&_replyBuffer[5],TimeOutms);
            memcpy(&_replyBuffer[1],&result,4);
            /// Dest byte, Result int, string, null char
            _replyBytes  = 6;
            _replyBytes += static_cast<unsigned int>(strlen(&_replyBuffer[5]));
            break;
        case ENUM_ListLocations :
            /// Send Code -- Get Dest, Result (int), nlocations (int), List (ints)
            ///result = _motionDirect.listLocations(&nLocations, List);
            ///memcpy(_replyBuffer+1, &result,4);
            ///memcpy(_replyBuffer+1+4,&nLocations,4);
            ///memcpy(_replyBuffer+1+8,List,nLocations*4);
            ///_replyBytes = 1+4+4+4*nLocations; // Dest byte, Result int, string, null char
            break;
        case ENUM_Failed:
            result = _motionDirect.failed();
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_Disconnect:
            result = _motionDirect.disconnect();
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_FirmwareVersion:
            result = _motionDirect.firmwareVersion();
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_CheckForReady:
            result = _motionDirect.checkForReady();
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_KMotionLock:
            result = _motionDirect.motionLock(&_requestBuffer[8]);
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_USBLocation:
            //result = _motionDirect.usbLocation();
            //memcpy(&_replyBuffer[1],&result,4);
            //_replyBytes = 5;
            break;
        case ENUM_KMotionLockRecovery:
            result = _motionDirect.motionLockRecovery();
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_ReleaseToken:
            _motionDirect.releaseToken();
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_ServiceConsole:
            result = _motionDirect.serviceConsole();
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_SetConsole:
            /// remember which pipe is associated with the console handler for the board
            result = _motionDirect.setConsoleCallback(ConsoleHandler);
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        case ENUM_Echo :
            result = static_cast<int>(0xAA55AA55);
            memcpy(&_replyBuffer[1],&result,4);
            _replyBytes = 5;
            break;
        default :
            return(false);
    }
    return(true);
}
///-----------------------------------------------------------------------------
int ConsoleHandler(const char *buf)
{
    ConsoleList.append(buf);
    return 0;
}
///-----------------------------------------------------------------------------
