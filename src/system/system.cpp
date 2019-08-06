///-----------------------------------------------------------------------------
#include "system.h"
///-----------------------------------------------------------------------------
SYSTEM_SPACE::SystemClass::SystemClass()
{
    _planner = new PLANNER_SPACE::PlannerClass;
    _parser  = new PARSER_SPACE::ParserDataClass;
    _interpreter = new INTERPRETER_SPACE::InterpreterClass;
    _coordinate  = new COORD_MOTION_SPACE::CoordMotionClass;
}
///-----------------------------------------------------------------------------
SYSTEM_SPACE::SystemClass::~SystemClass()
{
    delete _planner;
    delete _parser;
    delete _interpreter;
    delete _coordinate;
}
///-----------------------------------------------------------------------------
