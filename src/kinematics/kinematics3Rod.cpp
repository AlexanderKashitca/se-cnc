///-----------------------------------------------------------------------------
#include "kinematics3Rod.h"
///-----------------------------------------------------------------------------
#define sqr(x) ((x)*(x))
///-----------------------------------------------------------------------------
KINEMATICS_SPACE::Kinematics3RodClass::Kinematics3RodClass()
{
	Act0Center.x = 12;
	Act0Center.y = 0;
	Act0Center.z = 0;

	Act1Center.x = 0;
	Act1Center.y = 12;
	Act1Center.z = 0;

	Act2Center.x = 11;
	Act2Center.y = 3;
	Act2Center.z = 12;

	// when actuator position is zero this is how long the rod is

	Act0Off = sqrt(sqr(Act0Center.x) + sqr(Act0Center.y) + sqr(Act0Center.z));  
	Act1Off = sqrt(sqr(Act1Center.x) + sqr(Act1Center.y) + sqr(Act1Center.z));
	Act2Off = sqrt(sqr(Act2Center.x) + sqr(Act2Center.y) + sqr(Act2Center.z));

    _motionParams.maxLinearLength = 0.25;  // limit the segment lengs for nonlinear systems
    _motionParams.maxRapidFRO     = 1.0;   // limit the increase in Rapid HW FRO
    _motionParams.useOnlyLinearSegments=true;
}
///-----------------------------------------------------------------------------
KINEMATICS_SPACE::Kinematics3RodClass::~Kinematics3RodClass()
{

}
///-----------------------------------------------------------------------------
int KINEMATICS_SPACE::Kinematics3RodClass::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts)
{
	// find lengths of each actuator

    //GeoCorrect(x,y,z,&x,&y,&z);

	double r0 = sqrt(sqr(x-Act0Center.x) + sqr(y-Act0Center.y) + sqr(z-Act0Center.z)) - Act0Off;
	double r1 = sqrt(sqr(x-Act1Center.x) + sqr(y-Act1Center.y) + sqr(z-Act1Center.z)) - Act1Off;
	double r2 = sqrt(sqr(x-Act2Center.x) + sqr(y-Act2Center.y) + sqr(z-Act2Center.z)) - Act2Off;

    Acts[0] = r0*_motionParams.countsPerInchX;
    Acts[1] = r1*_motionParams.countsPerInchY;
    Acts[2] = r2*_motionParams.countsPerInchZ;

    Acts[3] = a*_motionParams.countsPerInchA;
    Acts[4] = b*_motionParams.countsPerInchB;
    Acts[5] = c*_motionParams.countsPerInchC;

	return 0;
}
///-----------------------------------------------------------------------------
/// perform Inversion to go the other way
int KINEMATICS_SPACE::Kinematics3RodClass::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr)
{
///	return InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr);
}
///-----------------------------------------------------------------------------
