///-----------------------------------------------------------------------------
#include "coordinate.h"
#include "../motion/common.h"
///-----------------------------------------------------------------------------
CoordMotionClass::CoordMotionClass()
{
    _motion = new MotionDirectClass;
}
///-----------------------------------------------------------------------------
CoordMotionClass::~CoordMotionClass()
{
    delete _motion;
}
///-----------------------------------------------------------------------------
void CoordMotionClass::setAbort()
{
    _abort = true;
}
///-----------------------------------------------------------------------------
void CoordMotionClass::clearAbort()
{
    if(_abort)
    {
        /// TODO
    }
    _abort = false;
}
///-----------------------------------------------------------------------------
bool CoordMotionClass::getAbort()
{
    return(_abort);
}
///-----------------------------------------------------------------------------
void CoordMotionClass::setHalt()
{
    _halt = true;
}
///-----------------------------------------------------------------------------
void CoordMotionClass::clearHalt()
{
    if(_halt)
    {
        /// TODO
    }
    _halt = false;
}
///-----------------------------------------------------------------------------
bool CoordMotionClass::getHalt()
{
    return(_halt);
}
///-----------------------------------------------------------------------------
int CoordMotionClass::setMotionCmd(const char *s,BOOL FlushBeforeUnbufferedOperation)
{
    /// exit if we are simulating
    if(_simulate)
        return 0;
    if(FlushBeforeUnbufferedOperation)
    {
        //if (FlushSegments()) {SetAbort(); return 1;}
        //if (WaitForSegmentsFinished(TRUE)) {SetAbort(); return 1;}
    }
    if(_motion->writeLine(s))
    {
        setAbort();
        return 1;
    }
    return 0;
}
