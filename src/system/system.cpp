///-----------------------------------------------------------------------------
#include "system.h"
///-----------------------------------------------------------------------------
using namespace SYSTEM_SPACE;
using namespace INTERPRETER_SPACE;
using namespace COORD_MOTION_SPACE;
///-----------------------------------------------------------------------------
SystemClass::SystemClass()
{
    _planner = new PLANNER_SPACE::PlannerClass;
    _parser  = new PARSER_SPACE::ParserDataClass;
    _interpreter = new INTERPRETER_SPACE::InterpreterClass;
    _coordinate  = new COORD_MOTION_SPACE::CoordMotionClass;
    _debug = false;
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
void SystemClass::setDebug(bool enable)
{
    _debug = enable;
    _parser->setDebug(enable);
    _planner->setDebug(enable);
    _coordinate->setDebug(enable);
    _interpreter->setDebug(enable);
}
///-----------------------------------------------------------------------------
bool SystemClass::getDebug()
{
    return(_debug);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::initialization()
{
    /// initialization coordinate system
    if(_coordinate->initialization() != COORD_MOTION_OK)
    {
        return(SYSTEM_FAIL);
    }
    /// initialization interpreter
    if(_interpreter->initialization(&_interpreter_settings) != INTERPRETER_OK)
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
