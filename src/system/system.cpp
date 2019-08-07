///-----------------------------------------------------------------------------
#include "system.h"
///-----------------------------------------------------------------------------
using namespace SYSTEM_SPACE;
using namespace COORD_MOTION_SPACE;
///-----------------------------------------------------------------------------
SystemClass::SystemClass()
{
    _planner = new PLANNER_SPACE::PlannerClass;
    _parser  = new PARSER_SPACE::ParserDataClass;
    _interpreter = new INTERPRETER_SPACE::InterpreterClass;
    _coordinate  = new COORD_MOTION_SPACE::CoordMotionClass;
}
///-----------------------------------------------------------------------------
SystemClass::~SystemClass()
{
    delete _planner;
    delete _parser;
    delete _interpreter;
    delete _coordinate;
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::initialization()
{
    if(_coordinate->initialization() != COORD_MOTION_OK)
    {
        return(SYSTEM_FAIL);
    }


    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::reInitialization()
{

    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
