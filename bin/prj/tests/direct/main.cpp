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

    MotionDirectClass MotionDirect;

    if(!MotionDirect.getFtdiLibraryLoad())
    {
        qDebug() << "FTDI Library not loading" << endl;
    }
    else
    {
        qDebug() << "All Library is loaded" << endl;
    }


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
    }

    return a.exec();
}
///-----------------------------------------------------------------------------
