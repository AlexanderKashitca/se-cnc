///-----------------------------------------------------------------------------
#include <QCoreApplication>
///-----------------------------------------------------------------------------
#include "../../../src/system/system.h"
///-----------------------------------------------------------------------------
int main(int argc,char* argv[])
{
    QCoreApplication app(argc, argv);

    SYSTEM_SPACE::SystemClass system;


    system._interpreter_settings.tool_path = "/home/evil/Programming/Qt/se-cnc/src/system/settings";
    system._interpreter_settings.tool_name = "rs274ngc.tool_default";
    system._interpreter_settings.param_path = "/home/evil/Programming/Qt/se-cnc/src/system/settings";
    system._interpreter_settings.param_name = "rs274ngc.var";
    system._interpreter_settings.parogram_in_path = "/home/evil/Programming/Qt/se-cnc/src/system/settings";
    system._interpreter_settings.parogram_in_name = "rs274ngc.in";
    system._interpreter_settings.parogram_out_path = "/home/evil/Programming/Qt/se-cnc/src/system/settings";
    system._interpreter_settings.parogram_out_name = "rs274ngc.out";

    system.setDebug(true);

    system.initialization();

    return app.exec();
}
///-----------------------------------------------------------------------------
