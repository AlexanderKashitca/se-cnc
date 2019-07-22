///-----------------------------------------------------------------------------
#include "cannon_out.h"
///-----------------------------------------------------------------------------
CannonOutClass::CannonOutClass()
{

}
///-----------------------------------------------------------------------------
void CannonOutClass::InitCanon()
{

}
///-----------------------------------------------------------------------------
void CannonOutClass::SetOroginOffsets(double x,double y,double z,double a,double b,double c)
{
    fprintf(_outfile, "%5d ", _line_number++);
    print_nc_line_number();
    fprintf(_outfile, "SET_ORIGIN_OFFSETS(%.4f, %.4f, %.4f"
           ", %.4f"  /*AA*/
           ", %.4f"  /*BB*/
           ", %.4f"  /*CC*/
           ")\n", x, y, z
           , a  /*AA*/
           , b  /*BB*/
           , c  /*CC*/
           );
    _program_position_x = _program_position_x + _program_origin_x - x;
    _program_position_y = _program_position_y + _program_origin_y - y;
    _program_position_z = _program_position_z + _program_origin_z - z;
    _program_position_a = _program_position_a + _program_origin_a - a;/*AA*/
    _program_position_b = _program_position_b + _program_origin_b - b;/*BB*/
    _program_position_c = _program_position_c + _program_origin_c - c;/*CC*/
    _program_origin_x = x;
    _program_origin_y = y;
    _program_origin_z = z;
    _program_origin_a = a;  /*AA*/
    _program_origin_b = b;  /*BB*/
    _program_origin_c = c;  /*CC*/
}
///-----------------------------------------------------------------------------
void CannonOutClass::UseLengthUnits(CANON_UNITS u)
{
    if (in_unit==CANON_UNITS_INCHES)
      {
        PRINT0("USE_LENGTH_UNITS(CANON_UNITS_INCHES)\n");
        if (_length_unit_type==CANON_UNITS_MM)
      {
        _length_unit_type=CANON_UNITS_INCHES;
        _length_unit_factor=25.4;
        _program_origin_x=(_program_origin_x / 25.4);
        _program_origin_y=(_program_origin_y / 25.4);
        _program_origin_z=(_program_origin_z / 25.4);
        _program_position_x=(_program_position_x / 25.4);
        _program_position_y=(_program_position_y / 25.4);
        _program_position_z=(_program_position_z / 25.4);
      }
      }
    else if (in_unit == CANON_UNITS_MM)
      {
        PRINT0("USE_LENGTH_UNITS(CANON_UNITS_MM)\n");
        if (_length_unit_type == CANON_UNITS_INCHES)
      {
        _length_unit_type   = CANON_UNITS_MM;
        _length_unit_factor = 1.0;
        _program_origin_x   = (_program_origin_x * 25.4);
        _program_origin_y   = (_program_origin_y * 25.4);
        _program_origin_z   = (_program_origin_z * 25.4);
        _program_position_x = (_program_position_x * 25.4);
        _program_position_y = (_program_position_y * 25.4);
        _program_position_z = (_program_position_z * 25.4);
      }
      }
    else
      PRINT0("USE_LENGTH_UNITS(UNKNOWN)\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::SelectPlane(CANON_PLANE in_plane)
{
    PRINT1("SELECT_PLANE(CANON_PLANE_%s)\n",
       ((in_plane==CANON_PLANE_XY) ? "XY" :
            (in_plane==CANON_PLANE_YZ) ? "YZ" :
        (in_plane==CANON_PLANE_XZ) ? "XZ" : "UNKNOWN"));
    _active_plane=in_plane;
}
///-----------------------------------------------------------------------------
void CannonOutClass::SetTraverseRate(double rate)
{
    PRINT1("SET_TRAVERSE_RATE(%.4f)\n", rate);
    _traverse_rate = rate;
}
///-----------------------------------------------------------------------------
void CannonOutClass::StaightTraverse(double x,double y,double z,double a_position,double b_position,double c_position)
{
    fprintf(_outfile, "%5d ", _line_number++);
    print_nc_line_number();
    fprintf(_outfile, "STRAIGHT_TRAVERSE(%.4f, %.4f, %.4f"
           ", %.4f" /*AA*/
           ", %.4f" /*BB*/
           ", %.4f" /*CC*/
           ")\n", x, y, z
           , a /*AA*/
           , b /*BB*/
           , c /*CC*/
           );
    _program_position_x=x;
    _program_position_y=y;
    _program_position_z=z;
    _program_position_a=a; /*AA*/
    _program_position_b=b; /*BB*/
    _program_position_c=c; /*CC*/
}
///-----------------------------------------------------------------------------
void CannonOutClass::SetFeedRate(double rate)
{
    PRINT1("SET_FEED_RATE(%.4f)\n", rate);
    _feed_rate=rate;
}
///-----------------------------------------------------------------------------
void CannonOutClass::SetFeedReference(CANON_FEED_REFERENCE reference)
{
    PRINT1("SET_FEED_REFERENCE(%s)\n",
       (reference==CANON_WORKPIECE) ? "CANON_WORKPIECE" : "CANON_XYZ");
}
///-----------------------------------------------------------------------------
void CannonOutClass::SetMotionControlMode(CANON_MOTION_MODE mode)
{
    if (mode==CANON_EXACT_STOP)
      {
        PRINT0("SET_MOTION_CONTROL_MODE(CANON_EXACT_STOP)\n");
        _motion_mode = CANON_EXACT_STOP;
      }
    else if (mode == CANON_EXACT_PATH)
      {
        PRINT0("SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH)\n");
        _motion_mode=CANON_EXACT_PATH;
      }
    else if (mode == CANON_CONTINUOUS)
      {
        PRINT0("SET_MOTION_CONTROL_MODE(CANON_CONTINUOUS)\n");
        _motion_mode = CANON_CONTINUOUS;
      }
    else
      PRINT0("SET_MOTION_CONTROL_MODE(UNKNOWN)\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::SetCutterRadiusCompensation(double radius)
{
PRINT1("SET_CUTTER_RADIUS_COMPENSATION(%.4f)\n", radius);
}
///-----------------------------------------------------------------------------
void CannonOutClass::StartCutterRadiusCompensation(int direction)
{
    PRINT1("START_CUTTER_RADIUS_COMPENSATION(%s)\n",
        (side==CANON_SIDE_LEFT)  ? "LEFT"  :
        (side==CANON_SIDE_RIGHT) ? "RIGHT" : "UNKNOWN");
}
///-----------------------------------------------------------------------------
void CannonOutClass::StopCutterRadiusCompensation()
{
PRINT0 ("STOP_CUTTER_RADIUS_COMPENSATION()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::StartSpeedFeedSynch()
{
PRINT0 ("START_SPEED_FEED_SYNCH()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::StopSpeedFeedSynch()
{
PRINT0 ("STOP_SPEED_FEED_SYNCH()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::ArcFeed(
    double first_end,
    double second_end,
    double first_axis,
    double second_axis,
    int rotation,
    double axis_end_point,
    double a_position,
    double b_position,
    double c_position
)
{
    fprintf(_outfile, "%5d ", _line_number++);
    print_nc_line_number();
    fprintf(_outfile, "ARC_FEED(%.4f, %.4f, %.4f, %.4f, %d, %.4f"
           ", %.4f" /*AA*/
           ", %.4f" /*BB*/
           ", %.4f" /*CC*/
           ")\n", first_end, second_end, first_axis, second_axis,
       rotation, axis_end_point
       , a /*AA*/
           , b /*BB*/
           , c /*CC*/
       );
    if (_active_plane==CANON_PLANE_XY)
      {
        _program_position_x=first_end;
        _program_position_y=second_end;
        _program_position_z=axis_end_point;
      }
    else if (_active_plane==CANON_PLANE_YZ)
      {
        _program_position_x=axis_end_point;
        _program_position_y=first_end;
        _program_position_z=second_end;
      }
    else /* if (_active_plane==CANON_PLANE_XZ) */
      {
        _program_position_x=second_end;
        _program_position_y=axis_end_point;
        _program_position_z=first_end;
      }
    _program_position_a=a; /*AA*/
    _program_position_b=b; /*BB*/
    _program_position_c=c; /*CC*/
}
///-----------------------------------------------------------------------------
void CannonOutClass::StraightFeed(
 double x, double y, double z
 , double a_position
 , double b_position
 , double c_position
)
{
    fprintf(_outfile, "%5d ", _line_number++);
    print_nc_line_number();
    fprintf(_outfile, "STRAIGHT_FEED(%.4f, %.4f, %.4f"
           ", %.4f" /*AA*/
           ", %.4f" /*BB*/
           ", %.4f" /*CC*/
           ")\n", x, y, z
           , a /*AA*/
           , b /*BB*/
           , c /*CC*/
           );
    _program_position_x=x;
    _program_position_y=y;
    _program_position_z=z;
    _program_position_a=a; /*AA*/
    _program_position_b=b; /*BB*/
    _program_position_c=c; /*CC*/
}
///-----------------------------------------------------------------------------
void CannonOutClass::StraightProbe(
 double x, double y, double z
 , double a_position
 , double b_position
 , double c_position
)
{
    double distance;
    double dx, dy, dz;
    double backoff;

    dx=(_program_position_x - x);
    dy=(_program_position_y - y);
    dz=(_program_position_z - z);
    distance=sqrt((dx * dx) + (dy * dy) + (dz * dz));
   fprintf(_outfile, "%5d ", _line_number++);
    print_nc_line_number();
    fprintf(_outfile, "STRAIGHT_PROBE(%.4f, %.4f, %.4f"
           ", %.4f" /*AA*/
           ", %.4f" /*BB*/
           ", %.4f" /*CC*/
           ")\n", x, y, z
           , a /*AA*/
           , b /*BB*/
           , c /*CC*/
           );
    _probe_position_x=x;
    _probe_position_y=y;
    _probe_position_z=z;
    _probe_position_a=a; /*AA*/
    _probe_position_b=b; /*BB*/
    _probe_position_c=c; /*CC*/
    if (distance == 0.0)
      {
        ///_program_position_x=_program_position_x;
        ///_program_position_y=_program_position_y;
        ///_program_position_z=_program_position_z;
      }
    else
      {
        backoff=((_length_unit_type==CANON_UNITS_MM) ? 0.254 : 0.01);
        _program_position_x=(x + (backoff * (dx / distance)));
        _program_position_y=(y + (backoff * (dy / distance)));
        _program_position_z=(z + (backoff * (dz / distance)));
      }
    _program_position_a=a; /*AA*/
    _program_position_b=b; /*BB*/
    _program_position_c=c; /*CC*/
}
///-----------------------------------------------------------------------------
void CannonOutClass::Stop()
{

}
///-----------------------------------------------------------------------------
void CannonOutClass::Dwell(double seconds)
{
PRINT1("DWELL(%.4f)\n", seconds);
}
///-----------------------------------------------------------------------------
void CannonOutClass::SpindleRetractTraverse()
{
    PRINT0("SPINDLE_RETRACT_TRAVERSE()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::StartSpindleClockwise()
{
    PRINT0("START_SPINDLE_CLOCKWISE()\n");
    _spindle_turning=((_spindle_speed==0.0) ? CANON_STOPPED :
                                         CANON_CLOCKWISE);
}
///-----------------------------------------------------------------------------
void CannonOutClass::StartSpindleCounterClockwise()
{
    PRINT0("START_SPINDLE_COUNTERCLOCKWISE()\n");
    _spindle_turning=((_spindle_speed==0.0) ? CANON_STOPPED :
                                         CANON_COUNTERCLOCKWISE);
}
///-----------------------------------------------------------------------------
void CannonOutClass::SetSpindleSpeed(double rpm)
{
    PRINT1("SET_SPINDLE_SPEED(%.4f)\n", rpm);
    _spindle_speed = rpm;
}
///-----------------------------------------------------------------------------
void CannonOutClass::StopSpindleTurning()
{
    PRINT0("STOP_SPINDLE_TURNING()\n");
    _spindle_turning=CANON_STOPPED;
}
///-----------------------------------------------------------------------------
void CannonOutClass::SpindleRetract()
{
PRINT0("SPINDLE_RETRACT()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::OrientSpindle(double orientation, CANON_DIRECTION direction)
{
    PRINT2("ORIENT_SPINDLE(%.4f, %s)\n", orientation,
        (direction==CANON_CLOCKWISE) ? "CANON_CLOCKWISE" :
                                             "CANON_COUNTERCLOCKWISE");
}
///-----------------------------------------------------------------------------
void CannonOutClass::LockSpindleZ()
{

}
///-----------------------------------------------------------------------------
void CannonOutClass::UseSpindleForce()
{

}
///-----------------------------------------------------------------------------
void CannonOutClass::UseNoSpindleForce()
{
PRINT0("USE_NO_SPINDLE_FORCE()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::UseToolLengthOffset(double length)
{
PRINT1("USE_TOOL_LENGTH_OFFSET(%.4f)\n", length);
}
///-----------------------------------------------------------------------------
/* slot is slot number */
void CannonOutClass::ChangeTool(int slot)
{
    PRINT1("CHANGE_TOOL(%d)\n", slot);
    _active_slot=slot;
}
///-----------------------------------------------------------------------------
/* i is slot number */
void CannonOutClass::SelectTool(int slot)
{
PRINT1("SELECT_TOOL(%d)\n", slot);
}
///-----------------------------------------------------------------------------
void CannonOutClass::ClampAxis(CANON_AXIS axis)
{
    PRINT1("CLAMP_AXIS(%s)\n",
        (axis==CANON_AXIS_X) ? "CANON_AXIS_X" :
        (axis==CANON_AXIS_Y) ? "CANON_AXIS_Y" :
        (axis==CANON_AXIS_Z) ? "CANON_AXIS_Z" :
        (axis==CANON_AXIS_A) ? "CANON_AXIS_A" :
        (axis==CANON_AXIS_C) ? "CANON_AXIS_C" : "UNKNOWN");
}
///-----------------------------------------------------------------------------
void CannonOutClass::Comment(char *s)
{
PRINT1("COMMENT(\"%s\")\n", s);
}
///-----------------------------------------------------------------------------
void CannonOutClass::DisableFeedOverride()
{
PRINT0("DISABLE_FEED_OVERRIDE()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::EnableFeedOverride()
{
PRINT0("ENABLE_FEED_OVERRIDE()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::DisableSpeedOverride()
{
PRINT0("DISABLE_SPEED_OVERRIDE()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::EnableSpeedOverride()
{
PRINT0("ENABLE_SPEED_OVERRIDE()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::FloodOff()
{
    PRINT0("FLOOD_OFF()\n");
    _flood=0;
}
///-----------------------------------------------------------------------------
void CannonOutClass::FloodOn()
{
    PRINT0("FLOOD_ON()\n");
    _flood=1;
}
///-----------------------------------------------------------------------------
void CannonOutClass::Message(char *s)
{
PRINT1("MESSAGE(\"%s\")\n", s);
}
///-----------------------------------------------------------------------------
void CannonOutClass::MistOff()
{
    PRINT0("MIST_OFF()\n");
    _mist=0;
}
///-----------------------------------------------------------------------------
void CannonOutClass::MistOn()
{
    PRINT0("MIST_ON()\n");
    _mist=1;
}
///-----------------------------------------------------------------------------
void CannonOutClass::PalletShuttle()
{
PRINT0("PALLET_SHUTTLE()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::TurnProbeOff()
{
PRINT0("TURN_PROBE_OFF()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::TurnProbeOn()
{
PRINT0("TURN_PROBE_ON()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::UnclampAxis(CANON_AXIS axis)
{
    PRINT1("UNCLAMP_AXIS(%s)\n",
        (axis==CANON_AXIS_X) ? "CANON_AXIS_X" :
        (axis==CANON_AXIS_Y) ? "CANON_AXIS_Y" :
        (axis==CANON_AXIS_Z) ? "CANON_AXIS_Z" :
        (axis==CANON_AXIS_A) ? "CANON_AXIS_A" :
        (axis==CANON_AXIS_B) ? "CANON_AXIS_B" :
        (axis==CANON_AXIS_C) ? "CANON_AXIS_C" : "UNKNOWN");
}
///-----------------------------------------------------------------------------
/* double knot values, -1.0 signals done */
void CannonOutClass::NurbKnotVector()
{

}
///-----------------------------------------------------------------------------
void CannonOutClass::NurbControlPoint(int i, double x, double y, double z, double w )
{

}
///-----------------------------------------------------------------------------
void CannonOutClass::NurbFeed(double sStart, double sEnd)
{

}
///-----------------------------------------------------------------------------
void CannonOutClass::OptionalProgramStop()
{
PRINT0("OPTIONAL_PROGRAM_STOP()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::ProgramEnd()
{
PRINT0("PROGRAM_END()\n");
}
///-----------------------------------------------------------------------------
void CannonOutClass::ProgramStop()
{
PRINT0("PROGRAM_STOP()\n");
}
///-----------------------------------------------------------------------------



///-----------------------------------------------------------------------------

/* Canonical "Do it" functions

This==a set of dummy definitions for the canonical machining functions
given in canon.hh. These functions just print themselves and, if necessary,
update the dummy world model. On each output line==printed:
1. an output line number (sequential, starting with 1).
2. an input line number read from the input (or ... if not provided).
3. a printed representation of the function call which was made.

If an interpreter which makes these calls==compiled with this set of
definitions, it can be used as a translator by redirecting output from
stdout to a file.

*/


#if 1

extern void rs274ngc_line_text(char * line_text, int max_size);

void print_nc_line_number()
{
  char text[256];
  int k;
  int m;

  rs274ngc_line_text(text, 256);
  for (k=0;
       ((k < 256) &&
    ((text[k]=='\t')||(text[k]==' ')||(text[k]=='/')));
       k++);
  if ((k < 256)&&((text[k]=='n')||(text[k]=='N')))
    {
      fputc('N', _outfile);
      for (k++, m=0;
       ((k < 256)&&(text[k] >= '0')&&(text[k] <= '9'));
       k++, m++)
    fputc(text[k], _outfile);
      for (; m < 6; m++)
    fputc(' ', _outfile);
    }
  else if (k < 256)
    fprintf(_outfile, "N..... ");
}
///-----------------------------------------------------------------------------
#define PRINT0(control) if (1)                        \
          {fprintf(_outfile, "%5d ", _line_number++); \
           print_nc_line_number();                    \
           fprintf(_outfile, control);                \
          }/// else
#define PRINT1(control, arg1) if (1)                  \
          {fprintf(_outfile, "%5d ", _line_number++); \
           print_nc_line_number();                    \
           fprintf(_outfile, control, arg1);          \
          }/// else
#define PRINT2(control, arg1, arg2) if (1)            \
          {fprintf(_outfile, "%5d ", _line_number++); \
           print_nc_line_number();                    \
           fprintf(_outfile, control, arg1, arg2);    \
          }/// else
#define PRINT3(control, arg1, arg2, arg3) if (1)         \
          {fprintf(_outfile, "%5d ", _line_number++);    \
           print_nc_line_number();                       \
           fprintf(_outfile, control, arg1, arg2, arg3); \
          }/// else
#define PRINT4(control, arg1, arg2, arg3, arg4) if (1)         \
          {fprintf(_outfile, "%5d ", _line_number++);          \
           print_nc_line_number();                             \
           fprintf(_outfile, control, arg1, arg2, arg3, arg4); \
          }/// else
#define PRINT5(control, arg1, arg2, arg3, arg4, arg5) if (1)         \
          {fprintf(_outfile, "%5d ", _line_number++);                \
           print_nc_line_number();                                   \
           fprintf(_outfile, control, arg1, arg2, arg3, arg4, arg5); \
          }/// else
#define PRINT6(control, arg1, arg2, arg3, arg4, arg5, arg6) if (1)         \
          {fprintf(_outfile, "%5d ", _line_number++);                      \
           print_nc_line_number();                                         \
           fprintf(_outfile, control, arg1, arg2, arg3, arg4, arg5, arg6); \
          }/// else
#define PRINT7(control, arg1, arg2, arg3, arg4, arg5, arg6, arg7) if (1) \
          {fprintf(_outfile, "%5d ", _line_number++);                    \
           print_nc_line_number();                                       \
           fprintf(_outfile, control,                                    \
                           arg1, arg2, arg3, arg4, arg5, arg6, arg7);    \
          }/// else
#define PRINT10(control,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10) \
          if (1)                                                            \
          {fprintf(_outfile, "%5d ", _line_number++);                       \
           print_nc_line_number();                                          \
           fprintf(_outfile, control,                                       \
                   arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10);     \
          }/// else

#endif

///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
/*
void PARAMETRIC_2D_CURVE_FEED(FunctionPtr f1, FunctionPtr f2,
                  double start_parameter_value,
                  double end_parameter_value) {}

void PARAMETRIC_3D_CURVE_FEED(FunctionPtr xfcn, FunctionPtr yfcn,
    FunctionPtr zfcn, double start_parameter_value,
                  double end_parameter_value) {}
*/
///-----------------------------------------------------------------------------
