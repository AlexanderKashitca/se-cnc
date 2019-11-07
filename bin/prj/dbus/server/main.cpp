///-----------------------------------------------------------------------------
#include <QCoreApplication>
///-----------------------------------------------------------------------------
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>
///-----------------------------------------------------------------------------
#include "../../../../src/dbus/common.h"
#include "../../../../src/dbus/server/server.h"

using namespace DBUS_SPACE;
///-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QObject obj;
    dbusServerClass *server = new dbusServerClass(&obj);
    QObject::connect(&app,&QCoreApplication::aboutToQuit,server,&dbusServerClass::aboutToQuit);
    server->setProperty("value", "initial value");
    QDBusConnection::sessionBus().registerObject("/",&obj);

    if(!QDBusConnection::sessionBus().registerService(SERVICE_NAME))
    {
        fprintf(stderr,"%s\n",qPrintable(QDBusConnection::sessionBus().lastError().message()));
        exit(1);
    }
    fprintf(stdout,"All Okay.\n");

    app.exec();
    return 0;
}
///-----------------------------------------------------------------------------
