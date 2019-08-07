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
            PLANNER_SPACE::PlannerClass* _planner;
            PARSER_SPACE::ParserDataClass* _parser;
            INTERPRETER_SPACE::InterpreterClass* _interpreter;
            COORD_MOTION_SPACE::CoordMotionClass* _coordinate;
        public :
            SystemClass();
            ~SystemClass();
            SYSTEM_STATE initialization();
            SYSTEM_STATE reInitialization();
    };
} /// end SYSTEM_SPACE
///-----------------------------------------------------------------------------
#endif // SYSTEM_H
///-----------------------------------------------------------------------------
