///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include <QDebug>
///-----------------------------------------------------------------------------
#include "../../../src/interpreter/interpreter.h"
///-----------------------------------------------------------------------------
using namespace INTERPRETER_SPACE;
///-----------------------------------------------------------------------------
const QString tool_path = "/home/evil/Programming/Qt/se-cnc/bin/build/build-interpreter-Desktop_Qt_5_12_3_GCC_64bit-Debug";
const QString tool_name = "rs274ngc.tool_default";

const QString param_path = "/home/evil/Programming/Qt/se-cnc/bin/build/build-interpreter-Desktop_Qt_5_12_3_GCC_64bit-Debug";
const QString param_name = "rs274ngc.var";

const QString out_path = "/home/evil/Programming/Qt/se-cnc/bin/build/build-interpreter-Desktop_Qt_5_12_3_GCC_64bit-Debug";
const QString out_name = "rs274ngc.out";

const QString in_path = "/home/evil/Programming/Qt/se-cnc/bin/build/build-interpreter-Desktop_Qt_5_12_3_GCC_64bit-Debug";
const QString in_name = "rs274ngc.in";
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    INTERPRETER_STATE status;
    INTERPRETER_SETTINGS settings;

    QCoreApplication app(argc, argv);

    InterpreterClass ipterpreter;

    settings.tool_path = tool_path;
    settings.tool_name = tool_name;
    settings.param_path = param_path;
    settings.param_name = param_name;
    settings.parogram_in_path = in_path;
    settings.parogram_in_name = in_name;
    settings.parogram_out_path = out_path;
    settings.parogram_out_name = out_name;

    ipterpreter.setDebug(true);

    status = ipterpreter.initialization(&settings);
    if(status == INTERPRETER_OK)
    {
        ipterpreter.setTolerance(2.0);
        status = ipterpreter.execute();
        qDebug() << "execution status - " << status << endl;
    }
    else{
        qDebug() << "Status - " << status << endl;
    }
    return app.exec();
}
///-----------------------------------------------------------------------------
