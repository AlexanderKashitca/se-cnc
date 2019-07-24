///-----------------------------------------------------------------------------
#ifndef CANNON_IN_OUT_H
#define CANNON_IN_OUT_H
///-----------------------------------------------------------------------------
#include "canon.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
///-----------------------------------------------------------------------------
class CannonInOutClass
{
    private :
        static CANON_PLANE       _active_plane;
        static int               _active_slot;
        static double            _feed_rate;
        static int               _flood;
        static double            _length_unit_factor;  /* 1 for MM 25.4 for inch */
        static CANON_UNITS       _length_unit_type;
        static int               _line_number;
        static int               _mist;
        static CANON_MOTION_MODE _motion_mode;
        static double            _probe_position_a;
        static double            _probe_position_b;
        static double            _probe_position_c;
        static double            _probe_position_x;
        static double            _probe_position_y;
        static double            _probe_position_z;
        static double            _program_origin_a;
        static double            _program_origin_b;
        static double            _program_origin_c;
        static double            _program_origin_x;
        static double            _program_origin_y;
        static double            _program_origin_z;
        static double            _program_position_a;
        static double            _program_position_b;
        static double            _program_position_c;
        static double            _program_position_x;
        static double            _program_position_y;
        static double            _program_position_z;
        static double            _spindle_speed;
        static CANON_DIRECTION   _spindle_turning;
        static double            _traverse_rate;
    public:
        char _parameter_file_name[100];          /// Not static.Driver writes
        int              _tool_max = 68;         /// Not static. Driver reads
        CANON_TOOL_TABLE _tools[CANON_TOOL_MAX]; /// Not static. Driver writes


        void SetOutFile(FILE* file);
    private:
        void print_nc_line_number();
        FILE* _outfile;

    public:
        CannonInOutClass();
        CannonInOutClass(const CannonInOutClass&);
        ~CannonInOutClass();


        void InitCanon();
        void SetOriginOffsets(double x,double y,double z,double a,double b,double c);
        void UseLengthUnits(CANON_UNITS in_unit);
        void SelectPlane(CANON_PLANE in_plane);
        void SetTraverseRate(double rate);
        void StraightTraverse(double x,double y,double z,double a_position,double b_position,double c_position);
        void SetFeedRate(double rate);
        void SetFeedReference(CANON_FEED_REFERENCE reference);
        void SetMotionControlMode(CANON_MOTION_MODE mode);
        void SetCutterRadiusCompensation(double radius);
        void StartCutterRadiusCompensation(int direction);
        void StopCutterRadiusCompensation();
        void StartSpeedFeedSynch();
        void StopSpeedFeedSynch();
        void ArcFeed(
            double first_end,
            double second_end,
            double first_axis,
            double second_axis,
            int rotation,
            double axis_end_point,
            double a_position,
            double b_position,
            double c_position
        );
        void StraightFeed(double x,double y,double z,double a_position,double b_position,double c_position);
        void StraightProbe(double x,double y,double z,double a_position,double b_position,double c_position);
        void Stop();
        void Dwell(double seconds);
        void SpindleRetractTraverse();
        void StartSpindleClockwise();
        void StartSpindleCounterClockwise();
        void SetSpindleSpeed(double r);
        void StopSpindleTurning();
        void SpindleRetract();
        void OrientSpindle(double orientation, CANON_DIRECTION direction);
        void LockSpindleZ();
        void UseSpindleForce();
        void UseNoSpindleForce();
        void UseToolLengthOffset(double length);
        void ChangeTool(int slot);
        void SelectTool(int i);
        void ClampAxis(CANON_AXIS axis);
        void Comment(char *s);
        void DisableFeedOverride();
        void EnableFeedOverride();
        void DisableSpeedOverride();
        void EnableSpeedOverride();
        void FloodOff();
        void FloodOn();
        void Message(char *s);
        void MistOff();
        void MistOn();
        void PalletShuttle();
        void TurnProbeOff();
        void TurnProbeOn();
        void UnclampAxis(CANON_AXIS axis);
        /* NURB Functions */
        void NurbKnotVector(); /* double knot values, -1.0 signals done */
        void NurbControlPoint(int i,double x,double y,double z,double w);
        void NurbFeed(double sStart,double sEnd);
        void OptionalProgramStop();
        void ProgramEnd();
        void ProgramStop();
        /// input part
        double GetExternalAngleUnitFactor();
        double GetExternalFeedRate();
        int GetExternalFlood();
        double GetExternalLengthUnitFactor();
        CANON_UNITS GetExternalLengthUnitType();
        int GetExternalMist();
        CANON_MOTION_MODE GetExternalMotionControlMode();
        double GetExternalOriginA();
        double GetExternalOriginB();
        double GetExternalOriginC();
        double GetExternalOriginX();
        double GetExternalOriginY();
        double GetExternalOriginZ();
        void GetExternalParameterFileName(char * filename, int max_size);
        CANON_PLANE GetExternalPlane();
        double GetExternalPositionA();
        double GetExternalPositionB();
        double GetExternalPositionC();
        double GetExternalPositionX();
        double GetExternalPositionY();
        double GetExternalPositionZ();
        double GetExternalProbePositionA();
        double GetExternalProbePositionB();
        double GetExternalProbePositionC();
        double GetExternalProbePositionX();
        double GetExternalProbePositionY();
        double GetExternalProbePositionZ();
        double GetExternalProbeValue();
        int GetExternalQueueEmpty();
        double GetExternalSpeed();
        CANON_DIRECTION GetExternalSpindle();
        // returns current tool length offset
        double GET_EXTERNAL_TOOL_LENGTH_OFFSET();
        int GetExternalToolMax();
        int GetExternalToolSlot();
        CANON_TOOL_TABLE GetExternalToolTable(int pocket);
        double GetExternalTraverseRate();
        ///---------------------------------------------------------------------
        void PRINT0(const char* control);
        void PRINT1(const char* control,const char* arg1);
};
///-----------------------------------------------------------------------------
#endif /// CANNON_IN_OUT_H
///-----------------------------------------------------------------------------
