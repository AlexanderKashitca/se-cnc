///-----------------------------------------------------------------------------
#include <QCoreApplication>
///-----------------------------------------------------------------------------
#include "../../../src/system/system.h"
///-----------------------------------------------------------------------------
int main(int argc,char* argv[])
{
    QCoreApplication app(argc, argv);

    SYSTEM_SPACE::SystemClass system;


    //system._interpreter_settings.tool_path = "";
    //system._interpreter_settings.tool_name = "";
    //system._interpreter_settings.param_path = "";
    //system._interpreter_settings.param_name = "";
    system._interpreter_settings.parogram_in_path = "";
    system._interpreter_settings.parogram_in_name = "";
    system._interpreter_settings.parogram_out_path = "/home/evil/Programming/Qt/se-cnc/src/system/settings";
    system._interpreter_settings.parogram_out_name = "rs274ngc.out";

    system.setDebug(true);


    system.setToolFile("/home/evil/Programming/Qt/se-cnc/src/system/settings/rs274ngc.tool_default");
    system.setParamFile("/home/evil/Programming/Qt/se-cnc/src/system/settings/rs274ngc.var");
    system.setProgramFile("/home/evil/Programming/Qt/se-cnc/src/system/settings/rs274ngc.in");

    system.initialization();
    system.runInterpreter();

    return app.exec();
}
///-----------------------------------------------------------------------------
