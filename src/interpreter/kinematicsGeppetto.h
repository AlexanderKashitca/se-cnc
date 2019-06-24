///-----------------------------------------------------------------------------
/// KinematicsGeppetto.h: interface for the CKinematicsGeppetto class.
///-----------------------------------------------------------------------------
#ifndef KINEMATICS_GEPPETTO_H
#define KINEMATICS_GEPPETTO_H
///-----------------------------------------------------------------------------
#include <math.h>
///-----------------------------------------------------------------------------
#include "kinematics.h"
///-----------------------------------------------------------------------------
class KinematicsGeppettoClass : public KinematicsClass
{
public:
	CPT3D Act0Center;
	CPT3D Act1Center;
	CPT3D Act2Center;
	CPT3D Act3Center;
	CPT3D Act4Center;
	CPT3D Act5Center;
	double Act0Off;
	double Act1Off;
	double Act2Off;
	double Act3Off;
	double Act4Off;
	double Act5Off;
    KinematicsGeppettoClass();
    virtual ~KinematicsGeppettoClass();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);
	virtual int ReadGeoTable(const char *name);
	int GeoCorrect(double x, double y, double z, double a, double b, double c,
		double *rx, double *ry, double *rz, double *ra, double *rb, double *rc);

	void Rotate(double xc,double yc,double zc,double z,double a,double b,double *xp,double *yp,double *zp);
	void Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
								double *xp,double *yp,double *zp);

	virtual int GetSoftLimits(double *xm, double *xp, double *ym, double *yp, double *zm, double *zp,
		double *am, double *ap, double *bm, double *bp, double *cm, double *cp);

};
///-----------------------------------------------------------------------------
#endif /// KINEMATICS_GEPPETTO_H
///-----------------------------------------------------------------------------
