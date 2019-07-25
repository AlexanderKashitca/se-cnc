///-----------------------------------------------------------------------------
#ifndef CANNON_IN_OUT_H
#define CANNON_IN_OUT_H
///-----------------------------------------------------------------------------
#include "canon.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
///-----------------------------------------------------------------------------
enum
{
    PARAMETER_FILE_NAME_SIZE = 255
};
///-----------------------------------------------------------------------------
class CannonInOutClass
{
    private :
        /// static member
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
        ///
        static void print_nc_line_number();
        static FILE* _outfile;
    public:
        static char _parameter_file_name[PARAMETER_FILE_NAME_SIZE];          /// Driver writes
        static int  _tool_max;                          /// Driver reads
        static CANON_TOOL_TABLE _tools[CANON_TOOL_MAX]; /// Driver writes
        static void SetOutFile(FILE* file);

        ///CannonInOutClass();
        ///CannonInOutClass(const CannonInOutClass&);
        ///virtual ~CannonInOutClass();




        static void InitCanon();
        static void SetOriginOffsets(double x,double y,double z,double a,double b,double c);
        static void UseLengthUnits(CANON_UNITS in_unit);
        static void SelectPlane(CANON_PLANE in_plane);
        static void SetTraverseRate(double rate);
        static void StraightTraverse(double x,double y,double z,double a_position,double b_position,double c_position);
        static void SetFeedRate(double rate);
        static void SetFeedReference(CANON_FEED_REFERENCE reference);
        static void SetMotionControlMode(CANON_MOTION_MODE mode);
        static void SetCutterRadiusCompensation(double radius);
        static void StartCutterRadiusCompensation(int direction);
        static void StopCutterRadiusCompensation();
        static void StartSpeedFeedSynch();
        static void StopSpeedFeedSynch();
        static void ArcFeed(
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
        static void StraightFeed(double x,double y,double z,double a_position,double b_position,double c_position);
        static void StraightProbe(double x,double y,double z,double a_position,double b_position,double c_position);
        static void Stop();
        static void Dwell(double seconds);
        static void SpindleRetractTraverse();
        static void StartSpindleClockwise();
        static void StartSpindleCounterClockwise();
        static void SetSpindleSpeed(double r);
        static void StopSpindleTurning();
        static void SpindleRetract();
        static void OrientSpindle(double orientation, CANON_DIRECTION direction);
        static void LockSpindleZ();
        static void UseSpindleForce();
        static void UseNoSpindleForce();
        static void UseToolLengthOffset(double length);
        static void ChangeTool(int slot);
        static void SelectTool(int i);
        static void ClampAxis(CANON_AXIS axis);
        static void Comment(char *s);
        static void DisableFeedOverride();
        static void EnableFeedOverride();
        static void DisableSpeedOverride();
        static void EnableSpeedOverride();
        static void FloodOff();
        static void FloodOn();
        static void Message(char *s);
        static void MistOff();
        static void MistOn();
        static void PalletShuttle();
        static void TurnProbeOff();
        static void TurnProbeOn();
        static void UnclampAxis(CANON_AXIS axis);
        /* NURB Functions */
        static void NurbKnotVector(); /* double knot values, -1.0 signals done */
        static void NurbControlPoint(int i,double x,double y,double z,double w);
        static void NurbFeed(double sStart,double sEnd);
        static void OptionalProgramStop();
        static void ProgramEnd();
        static void ProgramStop();
        /// input part
        static double GetExternalAngleUnitFactor();
        static double GetExternalFeedRate();
        static int GetExternalFlood();
        static double GetExternalLengthUnitFactor();
        static CANON_UNITS GetExternalLengthUnitType();
        static int GetExternalMist();
        static CANON_MOTION_MODE GetExternalMotionControlMode();
        static double GetExternalOriginA();
        static double GetExternalOriginB();
        static double GetExternalOriginC();
        static double GetExternalOriginX();
        static double GetExternalOriginY();
        static double GetExternalOriginZ();
        static void GetExternalParameterFileName(char * filename, int max_size);
        static CANON_PLANE GetExternalPlane();
        static double GetExternalPositionA();
        static double GetExternalPositionB();
        static double GetExternalPositionC();
        static double GetExternalPositionX();
        static double GetExternalPositionY();
        static double GetExternalPositionZ();
        static double GetExternalProbePositionA();
        static double GetExternalProbePositionB();
        static double GetExternalProbePositionC();
        static double GetExternalProbePositionX();
        static double GetExternalProbePositionY();
        static double GetExternalProbePositionZ();
        static double GetExternalProbeValue();
        static int GetExternalQueueEmpty();
        static double GetExternalSpeed();
        static CANON_DIRECTION GetExternalSpindle();
        // returns current tool length offset
        static double GET_EXTERNAL_TOOL_LENGTH_OFFSET();
        static int GetExternalToolMax();
        static int GetExternalToolSlot();
        static CANON_TOOL_TABLE GetExternalToolTable(int pocket);
        static double GetExternalTraverseRate();
        ///---------------------------------------------------------------------
        static void PRINT0(const char* control);
        static void PRINT1(const char* control,const char* arg1);
};
///-----------------------------------------------------------------------------
#endif /// CANNON_IN_OUT_H
///-----------------------------------------------------------------------------
