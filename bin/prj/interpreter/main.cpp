///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include <QDebug>
///-----------------------------------------------------------------------------
#include "../../../src/interpreter/interpreter.h"
///-----------------------------------------------------------------------------
const QString tool_path = "/home/evil/Programming/Qt/se-cnc/bin/build/build-interpreter-Desktop_Qt_5_12_3_GCC_64bit-Debug";
const QString tool_name = "rs274ngc.tool_default";

const QString out_path = "/home/evil/Programming/Qt/se-cnc/bin/build/build-interpreter-Desktop_Qt_5_12_3_GCC_64bit-Debug";
const QString out_name = "rs274ngc.out";

const QString in_path = "/home/evil/Programming/Qt/se-cnc/bin/build/build-interpreter-Desktop_Qt_5_12_3_GCC_64bit-Debug";
const QString in_name = "rs274ngc.in";
///-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    InterpreterClass ipterpreter;

    ipterpreter.SetToolFile(tool_path,tool_name);
    ipterpreter.ReadToolFile();

    ipterpreter.SetProgramInFile(in_path,in_name);
    ipterpreter.SetProgramOutFile(out_path,out_name);

    ipterpreter.Execute();

    return app.exec();
}
///-----------------------------------------------------------------------------
