///-----------------------------------------------------------------------------
#ifndef MOTION_PARAM_H
#define MOTION_PARAM_H
///-----------------------------------------------------------------------------
namespace PLANNER_SPACE
{
    typedef struct
    {
        double	breakAngle;
        double	collinearTol;
        double	cornerTol;
        double	facetAngle;
        double	tPLookahead;
        double	radiusA;
        double	radiusB;
        double	radiusC;

        double	maxAccelX;
        double	maxAccelY;
        double	maxAccelZ;
        double	maxAccelA;
        double	maxAccelB;
        double	maxAccelC;

        double	maxVelX;
        double	maxVelY;
        double	maxVelZ;
        double	maxVelA;
        double	maxVelB;
        double	maxVelC;

        double	maxRapidJerkX;  // used for 3rd order Rapids
        double	maxRapidJerkY;
        double	maxRapidJerkZ;
        double	maxRapidJerkA;
        double	maxRapidJerkB;
        double	maxRapidJerkC;

        double	maxRapidAccelX;
        double	maxRapidAccelY;
        double	maxRapidAccelZ;
        double	maxRapidAccelA;
        double	maxRapidAccelB;
        double	maxRapidAccelC;

        double	maxRapidVelX;
        double	maxRapidVelY;
        double	maxRapidVelZ;
        double	maxRapidVelA;
        double	maxRapidVelB;
        double	maxRapidVelC;

        double	softLimitNegX;  // Soft Limits (inches)
        double	softLimitNegY;
        double	softLimitNegZ;
        double	softLimitNegA;
        double	softLimitNegB;
        double	softLimitNegC;

        double	softLimitPosX;
        double	softLimitPosY;
        double	softLimitPosZ;
        double	softLimitPosA;
        double	softLimitPosB;
        double	softLimitPosC;

        double	countsPerInchX;
        double	countsPerInchY;
        double	countsPerInchZ;
        double	countsPerInchA;
        double	countsPerInchB;
        double	countsPerInchC;

        double	maxLinearLength;
        double	maxAngularChange;

        bool arcsToSegs;
        bool degreesA;
        bool degreesB;
        bool degreesC;

        bool useOnlyLinearSegments;
        bool doRapidsAsFeeds;

        double maxRapidFRO;

        bool TCP_Active;  // tool center point control active
        double TCP_X, TCP_Y, TCP_Z;  // tool center point offsets

    }MOTION_PARAMS;
}
///-----------------------------------------------------------------------------
#endif // MOTION_PARAM_H
///-----------------------------------------------------------------------------
