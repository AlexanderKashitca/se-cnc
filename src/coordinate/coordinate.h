///-----------------------------------------------------------------------------
#ifndef COORD_MOTION_H
#define COORD_MOTION_H
///-----------------------------------------------------------------------------
#include <linux/limits.h> /// MAX_PATH
#define MAX_PATH PATH_MAX
///-----------------------------------------------------------------------------
#include "../motion/direct.h"
#include "../interpreter/canon.h"
#include "../kinematics/kinematics.h"
///-----------------------------------------------------------------------------


///-----------------------------------------------------------------------------
/*
typedef void STRAIGHT_TRAVERSE_CALLBACK
        (
            double x,
            double y,
            double z,
            int sequence_number
        );
typedef void STRAIGHT_TRAVERSE_SIX_AXIS_CALLBACK
        (
            double x,
            double y,
            double z,
            double a,
            double b,
            double c,
            int sequence_number
        );
typedef void STRAIGHT_FEED_CALLBACK
        (
            double DesiredFeedRate_in_per_sec,
            double x,
            double y,
            double z,
            int sequence_number,
            int ID
        );
typedef void STRAIGHT_FEED_CALLBACK_SIX_AXIS
        (
            double DesiredFeedRate_in_per_sec,
            double x,
            double y,
            double z,
            double a,
            double b,
            double c,
            int sequence_number,
            int ID
        );
*/
typedef void ARC_FEED_CALLBACK
        (
            bool ZeroLenAsFullCircles,
            double DesiredFeedRate_in_per_sec,
            INTERPRETER_SPACE::CANON_PLANE plane,
            double first_end,
            double second_end,
            double first_axis,
            double second_axis,
            int rotation,
            double axis_end_point,
            double first_start,
            double second_start,
            double axis_start_point,
            int sequence_number,
            int ID
        );
typedef void ARC_FEED_SIX_AXIS_CALLBACK
        (
            bool ZeroLenAsFullCircles,
            double DesiredFeedRate_in_per_sec,
            INTERPRETER_SPACE::CANON_PLANE plane,
            double first_end,
            double second_end,
            double first_axis,
            double second_axis,
            int rotation,
            double axis_end_point,
            double a,
            double b,
            double c,
            double first_start,
            double second_start,
            double axis_start_point,
            int sequence_number,
            int ID
        );

///-----------------------------------------------------------------------------
namespace COORD_MOTION_SPACE
{
    using namespace MOTION_DIRECT_SPACE;

    enum
    {
        STOPPED_NONE,
        STOPPED_INDEP,
        STOPPED_COORD,
        STOPPED_COORD_FINISHED
    };
    typedef enum
    {
        COORD_MOTION_OK   = 0,
        COORD_MOTION_FAIL = 1
    }COORD_MOTION_RETVAL;

    class CoordMotionClass
    {
        private:
            MotionDirectClass* _motion;

            /// variables for _motion
            int     _result;
            QString _cmd;
            QString _response;

            QString _writeLineBuffer;
            double  _writeLineBufferTime;

            bool    _halt;
            bool    _abort;
            bool    _simulate;

            bool    _threadingMode;             /// Launches coordinated motion in spindle sync mode
            double  _threadingBaseSpeedRPS;    /// Base Rev/sec speed where trajectory should run an real-time

            bool    _defineCS_valid;
            /// map board channel number to interperter axis
            int     _x_axis;
            int     _y_axis;
            int     _z_axis;
            int     _a_axis;
            int     _b_axis;
            int     _c_axis;

            bool    _axisDisabled;


            double _feedRateOverride;
            double _feedRateRapidOverride;
            double _hardwareFRORange;
            double _spindleRateOverride;

            COORD_MOTION_RETVAL putWriteLineBuffer(QString s,double Time);
            COORD_MOTION_RETVAL flushWriteLineBuffer();
            COORD_MOTION_RETVAL clearWriteLineBuffer();
            COORD_MOTION_RETVAL launchCoordMotion();
            COORD_MOTION_RETVAL determineSoftwareHardwareFRO(double &HW,double &SW);


        public:
            CoordMotionClass();
            CoordMotionClass(const CoordMotionClass&) = delete;
            void operator= (const CoordMotionClass&)  = delete;
            ~CoordMotionClass();

            int initialization();
            KINEMATICS_SPACE::KinematicsClass* _kinematics;




            /// state
            void setAbort();
            void clearAbort();
            bool getAbort();
            void setHalt();
            void clearHalt();
            bool getHalt();

            COORD_MOTION_RETVAL setAxisDefinitions(int x,int y,int z,int a,int b,int c);
            COORD_MOTION_RETVAL getAxisDefinitions(int* x,int* y,int* z,int* a,int* b,int* c);

            COORD_MOTION_RETVAL getDestination(int axis,double *d);
            COORD_MOTION_RETVAL getPosition(int axis,double *d);

            COORD_MOTION_RETVAL getRapidSettings();
            COORD_MOTION_RETVAL getRapidSettingsAxis(int axis,double* Vel,double* Accel,double* Jerk,double* SoftLimitPos,double* SoftLimitNeg,double CountsPerInch);


            COORD_MOTION_RETVAL getAxisDone(int axis,int* r);
            COORD_MOTION_RETVAL setFeedRateOverride(double v);
            COORD_MOTION_RETVAL setFeedRateRapidOverride(double v);
            COORD_MOTION_RETVAL setHardwareFRORange(double v);
            COORD_MOTION_RETVAL setSpindleRateOverride(double v);
            double              getFeedRateOverride();
            double              getFeedRateRapidOverride();
            double              getSpindleRateOverride();
            double              getHardwareFRORange();


            COORD_MOTION_RETVAL setMotionCmd(const char *s,bool FlushBeforeUnbufferedOperation);



            //int checkMotionHalt(bool Coord);
            //int waitForMoveXYZABCFinished();
            //int waitForSegmentsFinished(bool NoErrorOnDisable = FALSE);

            //int DoKMotionCmd(const char *s, BOOL FlushBeforeUnbufferedOperation);
            //int DoKMotionBufCmd(const char *s,int sequence_number=-1);

            //int FlushSegments();
            //int LaunchCoordMotion();
            //int OutputSegment(int iseg);
            //int DownloadDoneSegments();

            //int FlushWriteLineBuffer();
            //int ConfigSpindle(int type, int axis, double UpdateTime, double Tau, double CountsPerRev);
            //int GetSpindleRPS(float &speed);

        /*

            void    DownloadInit();

            int     ExecutionStop();






        CoordMotionClass(MotionClass *KMotionDLL = new MotionClass(0));
        virtual ~CoordMotionClass();






        MOTION_PARAMS *GetMotionParams();

        int MeasurePointAppendToFile(const char *name);
        int StraightTraverse(double x, double y, double z, double a, double b, double c, bool NoCallback=false, int sequence_number=-1, int ID=0);
        int StraightTraverse(double x, double y, double z, double a, double b, double c, double u, double v, bool NoCallback=false, int sequence_number=-1, int ID=0);

        int ArcFeed(double DesiredFeedRate_in_per_sec, CANON_PLANE plane,
                    double first_end, double second_end,
                    double first_axis, double second_axis, int rotation,
                    double axis_end_point,
                    double a, double b, double c,
                    int sequence_number,
                    int ID);

        int ArcFeed(double DesiredFeedRate_in_per_sec, CANON_PLANE plane,
                    double first_end, double second_end,
                    double first_axis, double second_axis, int rotation,
                    double axis_end_point,
                    double a, double b, double c, double u, double v,
                    int sequence_number,
                    int ID);

        int ArcFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel, CANON_PLANE plane,
                    double first_end, double second_end,
                    double first_axis, double second_axis, int rotation,
                    double axis_end_point,
                    double a, double b, double c,
                    int sequence_number,
                    int ID);

        int ArcFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel, CANON_PLANE plane,
                    double first_end, double second_end,
                    double first_axis, double second_axis, int rotation,
                    double axis_end_point, double a, double b, double c, double u, double v, int sequence_number, int ID);

        int StraightFeed(double DesiredFeedRate_in_per_sec,
                         double x, double y, double z, double a, double b, double c, int sequence_number, int ID);

        int StraightFeed(double DesiredFeedRate_in_per_sec,
                         double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID);

        int StraightFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel,
                         double x, double y, double z, double a, double b, double c, int sequence_number, int ID);

        int StraightFeedAccel(double DesiredFeedRate_in_per_sec, double DesiredAccel,
                         double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID);

        int StraightFeedAccelRapid(double DesiredFeedRate_in_per_sec, double DesiredAccel, bool RapidMode, bool NoCallback,
                                   double x, double y, double z, double a, double b, double c, int sequence_number, int ID);

        int StraightFeedAccelRapid(double DesiredFeedRate_in_per_sec, double DesiredAccel, bool RapidMode, bool NoCallback,
                                   double x, double y, double z, double a, double b, double c, double u, double v, int sequence_number, int ID);


        int Dwell(double seconds, int sequence_number=0);

        int ReadCurAbsPosition(double *x, double *y, double *z, double *a, double *b, double *c, bool snap=false, bool NoGeo = false);
        int ReadCurAbsPosition(double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v, bool snap=false, bool NoGeo = false);

        void SetStraightTraverseCallback(STRAIGHT_TRAVERSE_CALLBACK *p);
        void SetStraightTraverseCallback(STRAIGHT_TRAVERSE_SIX_AXIS_CALLBACK *p);
        void SetStraightFeedCallback(STRAIGHT_FEED_CALLBACK *p);
        void SetStraightFeedCallback(STRAIGHT_FEED_CALLBACK_SIX_AXIS *p);
        void SetArcFeedCallback(ARC_FEED_CALLBACK *p);
        void SetArcFeedCallback(ARC_FEED_SIX_AXIS_CALLBACK *p);


        int DoSpecialCommand(int iseg);
        int DoSpecialInitialCommands();
        void DoSegmentCallbacks(int i0, int n);
        void DoSegmentCallbacksRapid(int i0, int i1);
        int DoRateAdjustments(int i0, int i1);
        int DoRateAdjustmentsArc(int i, double radius, double theta0, double dtheta, double dcircle);

        int CheckSoftLimits(double x, double y, double z, double a, double b, double c, double u, double v, QString &errmsg);
        int CheckSoftLimitsArc(double XC, double YC, double Z1,
                               double SoftLimitPosX,double SoftLimitNegX,
                               double SoftLimitPosY,double SoftLimitNegY,
                               double SoftLimitPosZ,double SoftLimitNegZ,
                               double a, double b, double c, double u, double v, BOOL DirIsCCW,
                               double radius, double theta0, double dtheta,
                               int x_axis,int y_axis,int z_axis,
                               char XSTR, char YSTR, char ZSTR, QString &errmsg);

        MotionClass *MotionLibrary;

        double m_TotalDownloadedTime;
        int m_nsegs_downloaded;
        double m_TimeAlreadyExecuted;

        int m_realtime_Sequence_number;  // latest sequence number where KFLOP is currently executing
        bool m_realtime_Sequence_number_valid;  // latest sequence number where KFLOP is currently executing is valid


        char MainPath[MAX_PATH],MainPathRoot[MAX_PATH];

        int m_board_type;

        bool m_Simulate;

        bool m_ThreadingMode;            // Launches coordinated motion in spindle sync mode
        double m_ThreadingBaseSpeedRPS;  // Base Rev/sec speed where trajectory should run an real-time

        bool m_DisableSoftLimits;

        bool m_AxisDisabled;

        bool m_TCP_affects_actuators;

        int m_Stopping;
        int m_PreviouslyStopped,m_PreviouslyStoppedType,m_PreviouslyStoppedID,m_PreviouslyStoppedSeqNo;

        double m_Stoppedx, m_Stoppedy, m_Stoppedz, m_Stoppeda, m_Stoppedb, m_Stoppedc, m_Stoppedu, m_Stoppedv;
        double m_StoppedMidx, m_StoppedMidy, m_StoppedMidz, m_StoppedMida, m_StoppedMidb, m_StoppedMidc, m_StoppedMidu, m_StoppedMidv;
        double m_StoppedMachinex, m_StoppedMachiney, m_StoppedMachinez, m_StoppedMachinea, m_StoppedMachineb, m_StoppedMachinec, m_StoppedMachineu, m_StoppedMachinev;



        bool m_DefineCS_valid;
        int x_axis,y_axis,z_axis,a_axis,b_axis,c_axis,u_axis,v_axis;  // map board channel number to interperter axis

        double current_x, current_y, current_z, current_a, current_b, current_c, current_u, current_v;

        STRAIGHT_TRAVERSE_CALLBACK *m_StraightTraverseCallback;
        STRAIGHT_TRAVERSE_SIX_AXIS_CALLBACK *m_StraightTraverseSixAxisCallback;
        STRAIGHT_FEED_CALLBACK *m_StraightFeedCallback;
        STRAIGHT_FEED_CALLBACK_SIX_AXIS *m_StraightFeedSixAxisCallback;

        void SetTPParams();


        bool RapidParamsDirty;

        void SetPreviouslyStoppedAtSeg(SEGMENT *segs_to_check,int i);

        double FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, BOOL *PureAngle);
        double FeedRateDistance(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, BOOL *PureAngle);


        bool m_TapCycleInProgress;

        bool CheckCollinear(SEGMENT *s0, SEGMENT *s1, SEGMENT *s2, double tol);
        */

    private:
        /*

        ARC_FEED_CALLBACK *m_ArcFeedCallback;
        ARC_FEED_SIX_AXIS_CALLBACK *m_ArcFeedSixAxisCallback;
        bool m_SegmentsStartedExecuting;
        int m_NumLinearNotDrawn;

        int CommitPendingSegments(bool RapidMode);
        int UpdateRealTimeState(double T);

        */
    };



} /// end namespace






///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
#endif /// COORD_MOTION_H
///-----------------------------------------------------------------------------

/*
1 N..... USE_LENGTH_UNITS(CANON_UNITS_MM)
2 N..... SET_ORIGIN_OFFSETS(11.0000, 12.0000, 13.0000, 14.0000, 15.0000, 16.0000)
3 N..... SET_FEED_REFERENCE(CANON_XYZ)
5 N..... SELECT_PLANE(CANON_PLANE_XZ)
6 N..... USE_LENGTH_UNITS(CANON_UNITS_MM)
7 N..... START_SPINDLE_CLOCKWISE()
11 N..... FLOOD_ON()
37 N..... SELECT_PLANE(CANON_PLANE_XZ)
61 N..... STRAIGHT_TRAVERSE(143.8280, -12.0000, -4.4120, -14.0000, -15.0000, -16.0000)
80 N..... STRAIGHT_FEED(134.0000, -12.0000, -4.6500, -14.0000, -15.0000, -16.0000)
99 N..... ARC_FEED(-84.4660, 131.0000, -45.0030, 153.9050, -1, -12.0000, -14.0000, -15.0000, -16.0000)
277 N..... MIST_OFF()
278 N..... FLOOD_OFF()
281 N..... STOP_SPINDLE_TURNING()
282 N..... SET_ORIGIN_OFFSETS(0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000)
283 N..... SELECT_PLANE(CANON_PLANE_XY)
284 N..... STOP_SPINDLE_TURNING()
285 N..... PALLET_SHUTTLE()
*/
