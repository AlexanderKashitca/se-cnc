///-----------------------------------------------------------------------------
#ifndef INTERPRETER_H
#define INTERPRETER_H
///-----------------------------------------------------------------------------
#include <QFile>
#include <QTextStream>
///-----------------------------------------------------------------------------
#include "rs274ngc.h"
///-----------------------------------------------------------------------------
namespace INTERPRETER_SPACE
{
    typedef enum
    {
        INTERPRETER_OK = 0,
        INTERPRETER_INIT,
        INTERPRETER_FILE_NOT_EXIST,
        INTERPRETER_FILE_NOT_OPEN,
        INTERPRETER_FILE_NOT_CREATE,
        INTERPRETER_FILE_ERROR
    }INTERPRETER_STATE;
    ///-------------------------------------------------------------------------
    class InterpreterClass
    {
        private :
            QString _tool_file;
            QString _parameter_file;
            QString _program_in_file;
            QString _program_out_file;
            QFile*  _output_program_file;
            int     _tool_flag;
            int     _gees[RS274NGC_ACTIVE_G_CODES];
            int     _ems[RS274NGC_ACTIVE_M_CODES];
            double  _sets[RS274NGC_ACTIVE_SETTINGS];
            bool    _debug;
            INTERPRETER_STATE LoadFromFile(int print_stack);
            void ReportError(int error_code,int print_stack);

        public :
            InterpreterClass();
            virtual ~InterpreterClass();
            InterpreterClass(const InterpreterClass&);

            void OnDebug();
            void OffDebug();
            void SetTolerance(double tolerance);

            INTERPRETER_STATE SetToolFile(QString file_path,QString file_name);
            INTERPRETER_STATE SetParameterFile(QString file_path,QString file_name);
            INTERPRETER_STATE SetProgramInFile(QString file_path,QString file_name);
            INTERPRETER_STATE SetProgramOutFile(QString file_path,QString file_name);

            INTERPRETER_STATE ReadToolFile();
            INTERPRETER_STATE Execute();

    };
}
///-----------------------------------------------------------------------------
#endif /// INTERPRETER_H
///-----------------------------------------------------------------------------
