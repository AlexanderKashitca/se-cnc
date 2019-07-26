///-----------------------------------------------------------------------------
#include <QCoreApplication>
#include <QDebug>
///-----------------------------------------------------------------------------
#include "../../../src/interpreter/interpreter.h"
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
    QCoreApplication app(argc, argv);

    InterpreterClass ipterpreter;

    status = ipterpreter.SetToolFile(tool_path,tool_name);
    status = ipterpreter.ReadToolFile();
    status = ipterpreter.SetParameterFile(param_path,param_name);
    status = ipterpreter.SetProgramInFile(in_path,in_name);
    status = ipterpreter.SetProgramOutFile(out_path,out_name);
    if(status == INTERPRETER_OK)
    {
        ipterpreter.SetTolerance(2.0);
        ipterpreter.Execute();
    }


    return app.exec();
}
///-----------------------------------------------------------------------------
