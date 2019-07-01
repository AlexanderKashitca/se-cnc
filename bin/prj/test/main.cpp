///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>
///-----------------------------------------------------------------------------
#include "../direct/direct.h"
///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int location[10];
    int list[10];
    QCoreApplication a(argc, argv);

    QLibrary library("libftd2xx.so");
    if (!library.load())
    qDebug() << library.errorString();
    else {
        qDebug() << "All Library is loaded" << endl;
    }

    MotionDirectClass MotionDirect;
    if(MotionDirect.listLocations(location,list) == 0)
    {
        int ret;
        ret = MotionDirect.motionLock();

        switch(ret)
        {
            case 0 : //SE_MOTION_LOCKED :
                qDebug() << "SE_MOTION_LOCKED" << endl;
                break;
            case 1 : //SE_MOTION_IN_USE :
                qDebug() << "SE_MOTION_IN_USE" << endl;
                break;
            case 2 : //SE_MOTION_NOT_CONNECTED :
                qDebug() << "SE_MOTION_NOT_CONNECTED" << endl;
                break;
        }

                /// returns 0 KMOTION_LOCKED (and token is locked) if KMotion is available for use
                /// returns 1 KMOTION_IN_USE if already in use
                /// returns 2 KMOTION_NOT_CONNECTED if not able to connect

    }

    return a.exec();
}
///-----------------------------------------------------------------------------
