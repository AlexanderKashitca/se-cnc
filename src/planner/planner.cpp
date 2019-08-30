///-----------------------------------------------------------------------------
#include "planner.h"
///-----------------------------------------------------------------------------
using namespace PLANNER_SPACE;
///-----------------------------------------------------------------------------
PlannerClass::PlannerClass()
{
    _reflexxesResultValue = 0;
    _current_x = 0.0;
    _current_y = 0.0;
    _current_z = 0.0;
    _current_a = 0.0;
    _current_b = 0.0;
    _current_c = 0.0;
}
///-----------------------------------------------------------------------------
PlannerClass::~PlannerClass()
{

}
///-----------------------------------------------------------------------------
/**
 * @brief PlannerClass::createReflexxes
 * @return
 */
PLANNER_STATE PlannerClass::createReflexxes(unsigned int DegreesOfFreedom,double CycleTimeInSeconds)
{
    _reflexxesDegreesOfFreedom   = DegreesOfFreedom;
    _reflexxesCycleTimeInSeconds = CycleTimeInSeconds;
    _REFLEXX_RML = new ReflexxesAPI(_reflexxesDegreesOfFreedom,_reflexxesCycleTimeInSeconds);
    _REFLEXX_IP  = new RMLPositionInputParameters(_reflexxesDegreesOfFreedom);
    _REFLEXX_OP  = new RMLPositionOutputParameters(_reflexxesDegreesOfFreedom);
    return(checkReflexxes());
}
///-----------------------------------------------------------------------------
/**
 * @brief PlannerClass::checkReflexxes
 * @return PLANNER_OK
 *         PLANNER_FAIL
 */
PLANNER_STATE PlannerClass::checkReflexxes()
{
    if(_REFLEXX_RML == nullptr || _REFLEXX_IP == nullptr || _REFLEXX_OP == nullptr)
        return(PLANNER_FAIL);
    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief PlannerClass::deleteReflexxes
 */
void PlannerClass::deleteReflexxes()
{
    delete _REFLEXX_RML;
    delete _REFLEXX_IP;
    delete _REFLEXX_OP;
}
///-----------------------------------------------------------------------------
void PlannerClass::setDebug(bool enable)
{
    _debug = enable;
}
///-----------------------------------------------------------------------------
bool PlannerClass::getDebug()
{
    return(_debug);
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesSetMaxVelocityVector(QVector<double> velocity)
{
    int axis;
    QVector<double>::iterator it_velocity;
    axis = 0;
    it_velocity = velocity.begin();
    while(it_velocity != velocity.end())
    {
        _REFLEXX_IP->MaxVelocityVector->VecData[axis] = *it_velocity;
        it_velocity++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesSetMaxAccelerationVector(QVector<double> acceleration)
{
    int axis;
    QVector<double>::iterator it_acceleration;
    axis = 0;
    it_acceleration = acceleration.begin();
    while(it_acceleration != acceleration.end())
    {
        _REFLEXX_IP->MaxAccelerationVector->VecData[axis] = *it_acceleration;
        it_acceleration++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesSetMaxJerkVector(QVector<double> jerk)
{
    int axis;
    QVector<double>::iterator it_jerk;
    axis = 0;
    it_jerk = jerk.begin();
    while(it_jerk != jerk.end())
    {
        _REFLEXX_IP->MaxAccelerationVector->VecData[axis] = *it_jerk;
        it_jerk++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesSetCurrentPosition(QVector<double> position)
{
    int axis;
    QVector<double>::iterator it_pos;
    axis = 0;
    it_pos = position.begin();
    while(it_pos != position.end())
    {
        _REFLEXX_IP->CurrentPositionVector->VecData[axis] = *it_pos;
        it_pos++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesSetCurrentVelocity(QVector<double> velocity)
{
    int axis;
    QVector<double>::iterator it_vel;
    axis = 0;
    it_vel = velocity.begin();
    while(it_vel != velocity.end())
    {
        _REFLEXX_IP->CurrentVelocityVector->VecData[axis] = *it_vel;
        it_vel++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesSetCurrentAcceleration(QVector<double> acceleration)
{
    int axis;
    QVector<double>::iterator it_accsel;
    axis = 0;
    it_accsel = acceleration.begin();
    while(it_accsel != acceleration.end())
    {
        _REFLEXX_IP->CurrentAccelerationVector->VecData[axis] = *it_accsel;
        it_accsel++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesSetTargetPosition(QVector<double> position)
{
    int axis;
    QVector<double>::iterator it_pos;
    axis = 0;
    it_pos = position.begin();
    while(it_pos != position.end())
    {
        _REFLEXX_IP->TargetPositionVector->VecData[axis] = *it_pos;
        it_pos++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesSetTargetVelocity(QVector<double> velocity)
{
    int axis;
    QVector<double>::iterator it_vel;
    axis = 0;
    it_vel = velocity.begin();
    while(it_vel != velocity.end())
    {
        _REFLEXX_IP->TargetVelocityVector->VecData[axis] = *it_vel;
        it_vel++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
void PlannerClass::reflexxesEnableAxis(QVector<bool> enable)
{
    int axis;
    QVector<bool>::iterator it_selection;
    axis = 0;
    it_selection = enable.begin();
    while(it_selection != enable.end())
    {
        _REFLEXX_IP->SelectionVector->VecData[axis] = *it_selection;
        it_selection++;
        axis++;
    }
}
///-----------------------------------------------------------------------------
/**
 * @brief PlannerClass::reflexxesValidity
 * @note  checking input parameters for reflexxess
 * @return true - successful
 */
bool PlannerClass::reflexxesValidity()
{
    return(_REFLEXX_IP->CheckForValidity());
}
///-----------------------------------------------------------------------------
/**
 * @brief PlannerClass::moveStraightTraverse
 * @param x - end coordinate axis x
 * @param y - end coordinate axis y
 * @param z - end coordinate axis z
 * @param a - end coordinate axis a
 * @param b - end coordinate axis b
 * @param c - end coordinate axis c
 * @param feed - feed to new point
 * @note Now realizate three axis
 */
PLANNER_STATE PlannerClass::moveStraightTraverse(double x,double y,double z,double a,double b,double c,double feed)
{
    if(createReflexxes(defaultDegreesOfFreedom,defaultCycleTimeInSeconds) != PLANNER_OK)
        return(PLANNER_FAIL);
    /// set current position
    _reflexPosition.clear();
    _reflexPosition.append(_current_x); /// begin x
    _reflexPosition.append(_current_y); /// begin y
    _reflexPosition.append(_current_z); /// begin z
    reflexxesSetCurrentPosition(_reflexPosition);
    /// set current position
    _reflexPosition.clear();
    _reflexPosition.append(x); /// end x
    _reflexPosition.append(y); /// end y
    _reflexPosition.append(z); /// end z
    reflexxesSetTargetPosition(_reflexPosition);
    /// set current feed for each axis
    _reflexVelocity.clear();
    _reflexPosition.append(0.0); /// current velocity x
    _reflexPosition.append(0.0); /// current velocity y
    _reflexPosition.append(0.0); /// current velocity z
    reflexxesSetCurrentVelocity(_reflexVelocity);
    /// set current acceleration for each axis
    _reflexAcceleration.clear();
    _reflexAcceleration.append(0.0); /// current acceleration x
    _reflexAcceleration.append(0.0); /// current acceleration y
    _reflexAcceleration.append(0.0); /// current acceleration z
    reflexxesSetCurrentAcceleration(_reflexAcceleration);
    /// set moving feed for each axis
    _reflexVelocity.clear();
    _reflexVelocity.append(100); /// velocity axis x
    _reflexVelocity.append(100); /// velocity axis y
    _reflexVelocity.append(100); /// velocity axis z
    reflexxesSetTargetVelocity(_reflexVelocity);
    /// set enable for each axis running moving operation
    _reflexEnable.clear();
    if(qFuzzyCompare(_current_x,x))
        _reflexEnable.append(false);
    else
        _reflexEnable.append(true);
    if(qFuzzyCompare(_current_y,y))
        _reflexEnable.append(false);
    else
        _reflexEnable.append(true);
    if(qFuzzyCompare(_current_z,z))
        _reflexEnable.append(false);
    else
        _reflexEnable.append(true);
    reflexxesEnableAxis(_reflexEnable);
    /// set maximum jerk vector
    _reflexJerk.clear();
    _reflexJerk.append(100.0); /// maximum jerk for axis x
    _reflexJerk.append(100.0); /// maximum jerk for axis y
    _reflexJerk.append(100.0); /// maximum jerk for axis z
    reflexxesSetMaxJerkVector(_reflexJerk);
    /// set maximum velocity vector
    _reflexVelocity.clear();
    _reflexVelocity.append(100.0); /// maximum velocity for axis x
    _reflexVelocity.append(100.0); /// maximum velocity for axis y
    _reflexVelocity.append(100.0); /// maximum velocity for axis z
    reflexxesSetMaxVelocityVector(_reflexVelocity);
    /// set maximum acceleration vector
    _reflexAcceleration.clear();
    _reflexAcceleration.append(100.0); /// maximum acceleration for axis x
    _reflexAcceleration.append(100.0); /// maximum acceleration for axis y
    _reflexAcceleration.append(100.0); /// maximum acceleration for axis z
    reflexxesSetMaxAccelerationVector(_reflexAcceleration);
    /// checked input parameters
    if(!reflexxesValidity())
        return(PLANNER_INVALID_PARAMETER);
    return(PLANNER_OK);
}
///-----------------------------------------------------------------------------



