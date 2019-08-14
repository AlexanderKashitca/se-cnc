///-----------------------------------------------------------------------------
#ifndef SYSTEM_H
#define SYSTEM_H
///-----------------------------------------------------------------------------
#include "../parser/parser.h"
#include "../planner/planner.h"
#include "../coordinate/coordinate.h"
#include "../interpreter/interpreter.h"
///-----------------------------------------------------------------------------
namespace SYSTEM_SPACE
{
    typedef enum
    {
        SYSTEM_OK   = 0,
        SYSTEM_FAIL = 1
    }SYSTEM_STATE;
    class SystemClass
    {
        private :
            bool _debug;

            PARSER_SPACE::ParserDataClass*        _parser;
            PLANNER_SPACE::PlannerClass*          _planner;
            COORD_MOTION_SPACE::CoordMotionClass* _coordinate;
            INTERPRETER_SPACE::InterpreterClass*  _interpreter;

        public :
            SystemClass();
            ~SystemClass();

            bool getDebug();
            void setDebug(bool enable);

            SYSTEM_STATE initialization();
            SYSTEM_STATE reInitialization();

            SYSTEM_STATE setToolFile(QString tool_file);
            SYSTEM_STATE setParamFile(QString param_file);
            SYSTEM_STATE setProgramFile(QString program_file);

            SYSTEM_STATE runParser();
            SYSTEM_STATE runInterpreter();



            INTERPRETER_SPACE::INTERPRETER_SETTINGS _interpreter_settings;
    };
} /// end SYSTEM_SPACE
///-----------------------------------------------------------------------------
#endif // SYSTEM_H
///-----------------------------------------------------------------------------
