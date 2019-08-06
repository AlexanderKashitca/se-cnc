///-----------------------------------------------------------------------------
#include "coordinate.h"
#include "../motion/common.h"
///-----------------------------------------------------------------------------
using namespace KINEMATICS_SPACE;
using namespace COORD_MOTION_SPACE;
using namespace MOTION_DIRECT_SPACE;
///-----------------------------------------------------------------------------
CoordMotionClass::CoordMotionClass()
{
    _kinematics = new KinematicsClass;
    _motion     = new MotionDirectClass;
}
///-----------------------------------------------------------------------------
CoordMotionClass::~CoordMotionClass()
{
    delete _motion;
    delete _kinematics;
}
///-----------------------------------------------------------------------------
int CoordMotionClass::initialization()
{
    /// TODO
    /// init direct channel

    return(0);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::putWriteLineBuffer(QString s,double Time)
{
    if(_abort)
    {
        return(COORD_MOTION_FAIL);
    }

    /// new string won't fit, flush it first
    if(_writeLineBuffer.length() + s.length() > (MAX_LINE - 10))
    {
        if(flushWriteLineBuffer() != COORD_MOTION_OK)
        {
            return(COORD_MOTION_FAIL);
        }
    }

    /// put in the string
    if(!_writeLineBuffer.isEmpty()) _writeLineBuffer += ';';
    _writeLineBuffer += s;
    _writeLineBufferTime += Time;

    /// If we have too much motion time in the buffer send it now
    /// allocate 10% of the Lookahead to be buffered here
    if (_writeLineBufferTime > _kinematics->_motionParams.tPLookahead * 0.1)
    {
        if(flushWriteLineBuffer())
        {
            return(COORD_MOTION_FAIL);
        }
    }
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::flushWriteLineBuffer()
{
    if(_abort)
    {
        return(COORD_MOTION_FAIL);
    }
    ///int Length = _writeLineBuffer.length();
    int result = _motion->writeLine(_writeLineBuffer.toStdString().c_str());
    clearWriteLineBuffer();
    if(result == 0)
        return(COORD_MOTION_OK);
    return(COORD_MOTION_FAIL);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::clearWriteLineBuffer()
{
    _writeLineBuffer.clear();
    _writeLineBufferTime = 0.0;
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::launchCoordMotion()
{
    QString response;
    response.clear();
    if(_motion->writeLineReadLine("CheckDoneBuf",response.toLatin1().begin()))
    {
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    if(response == "-1")
    {
        _axisDisabled = true;
        setAbort();
        return(COORD_MOTION_FAIL);
    }

    if(_threadingMode)  // Launch coordinated motion in spindle sync mode ?
    {
        if(fabs(_threadingBaseSpeedRPS) < 1e-9)
        {
            ///AfxMessageBox("Error Threading with Zero Speed");
            setAbort();
            return(COORD_MOTION_FAIL);
        }
        _cmd.clear();
        _cmd = QString("TrigThread %.6f").arg(_threadingBaseSpeedRPS);
        if(_motion->writeLine(_cmd.toStdString().c_str()))
        {
            setAbort();
            return(COORD_MOTION_FAIL);
        }
    }
    else  // no normal coordinated motion
    {
        if(_motion->writeLine("ExecBuf"))
        {
            setAbort();
            return(COORD_MOTION_FAIL);
        }
    }
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
/// Use a combination of Hardware and Software factors to
/// achieve the desired FRO.
///
/// Start at a SW=1.0 and HW=1.0 and gradually change one or the other
/// to move the current FRO toward our goal.  Whenever the total FRO
/// is above the HW Limit adjust the SW factor.  Whenever below the FRO
/// then adjust the HW Limit.
COORD_MOTION_RETVAL CoordMotionClass::determineSoftwareHardwareFRO(
        double &HW,
        double &SW)
{
    HW = 1.0;
    SW = 1.0;

    /// check if current FRO is above the HW limit
    if(1.0 > _hardwareFRORange)
    {
        /// yes it is above, SW should be used
        if(1.0 <= _feedRateOverride)  // need to increase?
        { /// yes, go all the way with SW
            SW = _feedRateOverride;
        }
        else
        { /// need to decrease
            /// check if decreasing all the way to desired FRO
            /// will put us below the HW Limit
            if(_feedRateOverride < _hardwareFRORange)
            {
                /// yes it will, decrease SW to limit
                SW = _hardwareFRORange;
                /// then remainder with HW
                HW = _feedRateOverride/SW;
            }
            else
            {
                /// no, do all with SW
                SW = _feedRateOverride;
            }
        }
    }
    else
    { /// Starting FRO=1.0 is below HW limit, HW should be used
        if(1.0 <= _feedRateOverride)  // need to increase (above 1.0)?
        {
            /// yes, check if increasing all the way to desired FRO
            /// will put us above the HW Limit?
            if(_feedRateOverride > _hardwareFRORange)
            {
                /// yes it will, increase HW to limit
                HW = _hardwareFRORange;
                /// then remainder with SW
                SW = _feedRateOverride / HW;
            }
            else
            { /// no we will still be below HW limit.  Use HW entirely
                HW = _feedRateOverride;
            }
        }
        else
        { /// need to decrease, do all with HW
            HW = _feedRateOverride;
        }
    }
    return(COORD_MOTION_OK);
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
COORD_MOTION_RETVAL CoordMotionClass::setMotionCmd(
                                        const char *s,
                                        bool FlushBeforeUnbufferedOperation)
{
    /// exit if we are simulating
    if(_simulate)
    {
        return(COORD_MOTION_OK);
    }
    if(FlushBeforeUnbufferedOperation)
    {
//        if(flushSegments())
//        {
//            setAbort();
//            return(COORD_MOTION_FAIL);
//        }
//        if(waitForSegmentsFinished(true))
//        {
//            setAbort();
//            return(COORD_MOTION_FAIL);
//        }
    }
    if(_motion->writeLine(s))
    {
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::setAxisDefinitions(int x,int y,int z,int a,int b,int c)
{
    _cmd.clear();
    _cmd = QString("DefineCS %d %d %d %d %d %d")
            .arg(x).arg(y).arg(z).arg(a).arg(b).arg(c);
    if(_motion->writeLine(_cmd.toStdString().c_str()))
    {
        return(COORD_MOTION_FAIL);
    }
    _x_axis = x;
    _y_axis = y;
    _z_axis = z;
    _a_axis = a;
    _b_axis = b;
    _c_axis = c;

    _defineCS_valid = true;

    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::getAxisDefinitions(int *x,int *y,int *z,int *a,int *b,int *c)
{
    if(!_defineCS_valid)
    {
        _cmd.clear();
        _cmd.append("DefineCS");
        if(_motion->writeLineReadLine(_cmd.toStdString().c_str(),_response.toLatin1().begin()))
        {
            return(COORD_MOTION_FAIL);
        }
        _result = sscanf(_response.toStdString().c_str(),
                            "%d%d%d%d%d%d",
                            &_x_axis,&_y_axis,&_z_axis,
                            &_a_axis,&_b_axis,&_c_axis
                        );
        if(_result != 6)
        {
            return(COORD_MOTION_FAIL);
        }
    }

    *x = _x_axis;
    *y = _y_axis;
    *z = _z_axis;
    *a = _a_axis;
    *b = _b_axis;
    *c = _c_axis;

    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::getDestination(int axis,double *d)
{
    *d = 0.0;

    /// not used in coordinate system
    if(axis == -1)
    {
        return(COORD_MOTION_OK);
    }

    if(axis < 0 || axis > N_CHANNELS)
    { /// invalid
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    /// send command
    _cmd.clear();
    _cmd = QString("Dest%d").arg(axis);
    if(_motion->writeLineReadLine(_cmd.toStdString().c_str(),_response.toLatin1().begin()))
    {
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    _result = sscanf(_response.toStdString().c_str(),"%lf",d);
    if(_result != 1)
    {
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::getPosition(int axis,double *d)
{
    *d=0.0;

    if(axis==-1)
    { /// not used in coordinate system
        return(COORD_MOTION_FAIL);
    }

    if(axis < 0 || axis > N_CHANNELS)
    { /// invalid
        setAbort();
        return(COORD_MOTION_FAIL);
    }

    _cmd.clear();
    _cmd = QString("Pos%d").arg(axis);
    if(_motion->writeLineReadLine(_cmd.toStdString().c_str(),_response.toLatin1().begin()))
    {
        setAbort();
        return(COORD_MOTION_FAIL);
    }

    _result = sscanf(_response.toStdString().c_str(),"%lf",d);
    if(_result != 1)
    {
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::getRapidSettings()
{

    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::getRapidSettingsAxis(
                                        int axis,
                                        double *Vel,
                                        double *Accel,
                                        double *Jerk,
                                        double *SoftLimitPos,
                                        double *SoftLimitNeg,
                                        double CountsPerInch)
{
    double temp;
    int timeout(100);

    if(CountsPerInch == 0.0)
    {
        return(COORD_MOTION_FAIL);
    }

    if(axis == -1)
    {
        return(COORD_MOTION_OK);
    }
    _cmd.clear();
    _cmd = QString("Vel%d;Accel%d;Jerk%d;SoftLimitPos%d;SoftLimitNeg%d")
                .arg(axis)
                .arg(axis)
                .arg(axis)
                .arg(axis)
                .arg(axis);
    if(_motion->writeLine(_cmd.toStdString().c_str()))
    {
        return(COORD_MOTION_FAIL);
    }

    if(_motion->readLineTimeOut(_response.toLatin1().begin(),timeout))
    {

    }
    _result = sscanf(_response.toStdString().c_str(),"%lf",&temp);
    if(_result != 1)
    {
         return(COORD_MOTION_FAIL);
    }
    *Vel = fabs(temp/CountsPerInch);

    if(_motion->readLineTimeOut(_response.toLatin1().begin(),timeout))
    {
         return(COORD_MOTION_FAIL);
    }

    _result = sscanf(_response.toStdString().c_str(),"%lf",&temp);
    if(_result != 1)
    {
         return(COORD_MOTION_FAIL);
    }
    *Accel = fabs(temp/CountsPerInch);

    if(_motion->readLineTimeOut(_response.toLatin1().begin(),timeout))
    {
        return(COORD_MOTION_FAIL);
    }

    _result = sscanf(_response.toStdString().c_str(),"%lf",&temp);
    if(_result != 1)
    {
        return(COORD_MOTION_FAIL);
    }
    *Jerk = fabs(temp/CountsPerInch);

    if(_motion->readLineTimeOut(_response.toLatin1().begin(),timeout))
    {
        return(COORD_MOTION_FAIL);
    }
    _result = sscanf(_response.toStdString().c_str(),"%lf",&temp);
    if(_result != 1)
    {
        return(COORD_MOTION_FAIL);
    }
    if(CountsPerInch >= 0.0)
        *SoftLimitPos = temp / CountsPerInch;
    else
        *SoftLimitNeg = temp / CountsPerInch;

    if(_motion->readLineTimeOut(_response.toLatin1().begin(),timeout))
    {
         return(COORD_MOTION_FAIL);
    }
    _result = sscanf(_response.toStdString().c_str(),"%lf",&temp);
    if(_result != 1)
    {
         return(COORD_MOTION_FAIL);
    }
    if(CountsPerInch >= 0.0)
        *SoftLimitNeg = temp / CountsPerInch;
    else
        *SoftLimitPos = temp / CountsPerInch;
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::getAxisDone(int axis,int* r)
{
    *r = -1; /// assume disabled

    if(axis == -1)
    { /// not used in coordinate system
        return(COORD_MOTION_OK);
    }

    if(axis < 0 || axis > N_CHANNELS)
    { /// invalid
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    _cmd.clear();
    _cmd = QString("CheckDone%d").arg(axis);
    if(_motion->writeLineReadLine(_cmd.toStdString().c_str(),_response.toLatin1().begin()))
    {
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    _result = sscanf(_response.toStdString().c_str(),"%d",r);
    if(_result != 1)
    {
        setAbort();
        return(COORD_MOTION_FAIL);
    }
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::setFeedRateOverride(double v)
{
    double HW,SW;

    _feedRateOverride = v;

    if(!_simulate)
    {
        determineSoftwareHardwareFRO(HW,SW);
        _cmd.clear();
        _cmd = QString("SetFRO %.4f").arg(HW);
        _motion->writeLine(_cmd.toStdString().c_str());
    }
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::setFeedRateRapidOverride(double v)
{
    if(v > _kinematics->_motionParams.maxRapidFRO)
    {
        v = _kinematics->_motionParams.maxRapidFRO;
    }
    _feedRateRapidOverride = v;
    if(!_simulate)
    {
        _cmd.clear();
        _cmd = QString("SetRapidFRO %.4f").arg(v);
        _motion->writeLine(_cmd.toStdString().c_str());
    }
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::setHardwareFRORange(double v)
{
    _hardwareFRORange = v;
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
COORD_MOTION_RETVAL CoordMotionClass::setSpindleRateOverride(double v)
{
    _spindleRateOverride = v;
    return(COORD_MOTION_OK);
}
///-----------------------------------------------------------------------------
double CoordMotionClass::getFeedRateOverride()
{
    return(_feedRateOverride);
}
///-----------------------------------------------------------------------------
double CoordMotionClass::getFeedRateRapidOverride()
{
    return(_feedRateRapidOverride);
}
///-----------------------------------------------------------------------------
double CoordMotionClass::getSpindleRateOverride()
{
    return(_spindleRateOverride);
}
///-----------------------------------------------------------------------------
double CoordMotionClass::getHardwareFRORange()
{
    return(_hardwareFRORange);
}
///-----------------------------------------------------------------------------
