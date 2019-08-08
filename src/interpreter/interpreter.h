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

    typedef struct
    {
        QString tool_path;
        QString tool_name;
        QString param_path;
        QString param_name;
        QString parogram_in_path;
        QString parogram_in_name;
        QString parogram_out_path;
        QString parogram_out_name;
    }INTERPRETER_SETTINGS;
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
            INTERPRETER_STATE loadFromFile(int print_stack);
            void              reportError(int error_code,int print_stack);
            INTERPRETER_STATE setToolFile(QString file_path,QString file_name);
            INTERPRETER_STATE setParameterFile(QString file_path,QString file_name);
            INTERPRETER_STATE setProgramInFile(QString file_path,QString file_name);
            INTERPRETER_STATE setProgramOutFile(QString file_path,QString file_name);
        public :
            InterpreterClass();
            virtual ~InterpreterClass();
            InterpreterClass(const InterpreterClass&);

            void setDebug(bool enable);
            bool getDebug();
            void setTolerance(double tolerance);

            INTERPRETER_STATE readToolFile();
            INTERPRETER_STATE initialization(INTERPRETER_SETTINGS* settings);
            INTERPRETER_STATE execute();

    };
}
///-----------------------------------------------------------------------------
#endif /// INTERPRETER_H
///-----------------------------------------------------------------------------
