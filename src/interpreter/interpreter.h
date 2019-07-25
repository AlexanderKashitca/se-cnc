///-----------------------------------------------------------------------------
#ifndef INTERPRETER_H
#define INTERPRETER_H
///-----------------------------------------------------------------------------
#include <QFile>
#include <QTextStream>
///-----------------------------------------------------------------------------
#include "rs274ngc.h"
///-----------------------------------------------------------------------------
typedef enum
{
    INTERPRETER_OK = 0,
    INTERPRETER_INIT,
    INTERPRETER_FILE_NOT_EXIST,
    INTERPRETER_FILE_NOT_OPEN,
    INTERPRETER_FILE_NOT_CREATE,
    INTERPRETER_FILE_ERROR
}INTERPRETER_STATE;
///-----------------------------------------------------------------------------
class InterpreterClass
{
    private :
        QString _tool_file;
        QString _parameter_file;
        QString _program_in_file;
        QString _program_out_file;

        int    _tool_flag;
        int    _gees[RS274NGC_ACTIVE_G_CODES];
        int    _ems[RS274NGC_ACTIVE_M_CODES];
        double _sets[RS274NGC_ACTIVE_SETTINGS];

    public :
        FILE*  _output_progra_file;

        InterpreterClass();
        InterpreterClass(const InterpreterClass&);

        INTERPRETER_STATE SetToolFile(QString file_path,QString file_name);
        INTERPRETER_STATE SetParameterFile(QString file_path,QString file_name);
        INTERPRETER_STATE SetProgramInFile(QString file_path,QString file_name);
        INTERPRETER_STATE SetProgramOutFile(QString file_path,QString file_name);

        INTERPRETER_STATE ReadToolFile();
        INTERPRETER_STATE Execute();

};
///-----------------------------------------------------------------------------
#endif /// INTERPRETER_H
///-----------------------------------------------------------------------------
