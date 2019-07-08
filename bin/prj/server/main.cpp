///-----------------------------------------------------------------------------
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>
///-----------------------------------------------------------------------------
#include "../../../src/server/server.h"
#include "../../../src/motion/motion_service.h"
///-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QObject obj;
    ServerClass server(&obj);

    QObject::connect(&app,&QCoreApplication::aboutToQuit,&server,&ServerClass::aboutToQuit);
    server.setProperty("value","initial value");
    QDBusConnection::sessionBus().registerObject("/",&obj);
    if (!QDBusConnection::sessionBus().registerService(SE_MOTION_SERVICE_NAME))
    {
        fprintf(stderr,"%s\n",qPrintable(QDBusConnection::sessionBus().lastError().message()));
        exit(1);
    }
    app.exec();
    return 0;
}
///-----------------------------------------------------------------------------
