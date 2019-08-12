///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include <QDebug>
///-----------------------------------------------------------------------------
#include "../../../src/motion/direct.h"
#include "../../../src/motion/loader.h"
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    LOADER_SPACE::LoaderClass loader;


    QString s;
    int Thread        = 1;
    const char Name[] = "/home/evil/Programming/Qt/se-cnc/bin/build/build-direct-Desktop_Qt_5_12_3_GCC_64bit-Debug/a.out";

    int PackToFlash   = 1;
    unsigned int EntryPoint;

    if(Thread == 0) return 1;



    if (PackToFlash == 0)
    {
        s = QString("Kill %d")
                .arg(Thread);  // make sure the Thread isn't running
        qDebug() << s;
        //if(WriteLine(s)) return 1;
    }

    int result = loader.load(Name, &EntryPoint, PackToFlash);

    if(result)
    {
        qDebug() << "result - " << result;
    }

    if(Thread >= 0 && PackToFlash == 0)
    {
        // Set the entry point for the thread

        //s.Format("EntryPoint%d %X",Thread,EntryPoint);
        s = QString("EntryPoint%d %X")
                .arg(Thread)
                .arg(EntryPoint);
        //result = WriteLine(s);
        qDebug() << s;
        if (result) return result;
    }

    return app.exec();
}
///-----------------------------------------------------------------------------
