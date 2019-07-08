#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtDBus/QtDBus>

#include "../dbus/common.h"
#include "ping.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return 1;
    }

    QDBusServiceWatcher serviceWatcher(SERVICE_NAME, QDBusConnection::sessionBus(),
                                       QDBusServiceWatcher::WatchForRegistration);

    Ping ping;
    QObject::connect(&serviceWatcher, &QDBusServiceWatcher::serviceRegistered,
                     &ping, &Ping::start);

    ping.start(SERVICE_NAME);
//    QProcess pong;
//    pong.start("./complexpong");

    app.exec();
}
