///-----------------------------------------------------------------------------
#ifndef KINEMATICS_H
#define KINEMATICS_H
///-----------------------------------------------------------------------------
#include <QString>
///-----------------------------------------------------------------------------
//#include "trajectoryPlanner.h"

///-----------------------------------------------------------------------------
/**
 * @brief The CPT3D class
 */
class CPT3D
{
    public:
        double y;
        double x;
        double z;
        double c;
        CPT3D();
        virtual ~CPT3D();
};
///-----------------------------------------------------------------------------
/**
 * @brief The CPT2D class
 */
class CPT2D
{
    public:
        double y;
        double x;
        CPT2D();
        virtual ~CPT2D();
};
///-----------------------------------------------------------------------------
#define MAX_ACTUATORS 8
///-----------------------------------------------------------------------------
class KinematicsClass
{
    public:
        int Solve(double *A, int N);
        int MaxAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel);
        int MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate);
        int MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double *rate);
        int MaxRapidRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate);
        int MaxRapidJerkInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *jerk);
        int MaxRapidAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel);
        virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double u, double v, double *Acts, bool NoGeo = false);
        virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
        virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
        virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v, bool NoGeo = false);
        virtual int ComputeAnglesOption(int is);
        int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);
        virtual int RemapForNonStandardAxes(double *x, double *y, double *z, double *a, double *b, double *c);

    ///    int IntersectionTwoCircles(CPT2D c0, double r0, CPT2D c1, double r1, CPT2D *q);

        virtual int ReadGeoTable(const char *name);
        virtual int GeoCorrect(double x, double y, double z, double *cx, double *cy, double *cz);
        virtual int GetSoftLimits(
                        double *xm, double *xp,
                        double *ym, double *yp,
                        double *zm, double *zp,
                        double *am, double *ap,
                        double *bm, double *bp,
                        double *cm, double *cp,
                        double *um, double *up,
                        double *vm, double *vp
                    ) = 0;

	
    KinematicsClass();
    virtual ~KinematicsClass();

	MOTION_PARAMS m_MotionParams;


	bool GeoTableValid;
	CPT3D *GeoTable;
    QString *Table2;
	int NRows,NCols;
	double GeoSpacingX,GeoSpacingY;
	double GeoOffsetX,GeoOffsetY;  // Machine coordinates of grid point row=0 col=0

};
///-----------------------------------------------------------------------------
#endif /// KINEMATICS_H
///-----------------------------------------------------------------------------