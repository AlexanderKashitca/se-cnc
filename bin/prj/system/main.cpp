///-----------------------------------------------------------------------------
#include <QCoreApplication>
///-----------------------------------------------------------------------------
#include "../../../src/system/system.h"
///-----------------------------------------------------------------------------
int main(int argc,char* argv[])
{
    QCoreApplication app(argc, argv);

    SYSTEM_SPACE::SystemClass system;

    system.initialization();

    return app.exec();
}
///-----------------------------------------------------------------------------
