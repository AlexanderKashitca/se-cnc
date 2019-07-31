///-----------------------------------------------------------------------------
#include "planner.h"
///-----------------------------------------------------------------------------
PlannerClass::PlannerClass()
{
    _reflexxesDegreesOfFreedom   = 3;
    _reflexxesCycleTimeInSeconds = 0.001;

    _reflexxesResultValue = 0;

    _REFLEXX_RML = new ReflexxesAPI(_reflexxesDegreesOfFreedom,_reflexxesCycleTimeInSeconds);
    _REFLEXX_IP  = new RMLPositionInputParameters(_reflexxesDegreesOfFreedom);
    _REFLEXX_OP  = new RMLPositionOutputParameters(_reflexxesDegreesOfFreedom);
}
///-----------------------------------------------------------------------------
PlannerClass::~PlannerClass()
{
    delete _REFLEXX_RML;
    delete _REFLEXX_IP;
    delete _REFLEXX_OP;
}
///-----------------------------------------------------------------------------
/*
_REFLEXX_IP->MaxVelocityVector->VecData          [0] =    100.0      ;
_REFLEXX_IP->MaxVelocityVector->VecData          [1] =    100.0      ;
_REFLEXX_IP->MaxVelocityVector->VecData          [2] =    100.0      ;

_REFLEXX_IP->MaxAccelerationVector->VecData      [0] =    100.0      ;
_REFLEXX_IP->MaxAccelerationVector->VecData      [1] =    100.0      ;
_REFLEXX_IP->MaxAccelerationVector->VecData      [2] =    100.0      ;

_REFLEXX_IP->MaxJerkVector->VecData              [0] =    100.0      ;
_REFLEXX_IP->MaxJerkVector->VecData              [1] =    100.0      ;
_REFLEXX_IP->MaxJerkVector->VecData              [2] =    100.0      ;
*/
void PlannerClass::ReflexxesSetCurrentPosition(QVector<double> position)
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
void PlannerClass::ReflexxesSetCurrentVelocity(QVector<double> velocity)
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
void PlannerClass::ReflexxesSetCurrentAcceleration(QVector<double> acceleration)
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
void PlannerClass::ReflexxesSetTargetPosition(QVector<double> position)
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
void PlannerClass::ReflexxesSetTargetVelocity(QVector<double> velocity)
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
void PlannerClass::ReflexxesSetSelection(QVector<bool> enable)
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
bool PlannerClass::ReflexxesValidity()
{
    return(_REFLEXX_IP->CheckForValidity());
}
///-----------------------------------------------------------------------------

