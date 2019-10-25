#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtDBus/QtDBus>

#include "../../../../src/dbus/common.h"
#include "../../../../src/dbus/client/client.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);


    dbusClientClass ping;


    ping.start(SERVICE_NAME);


    app.exec();
}
