///-----------------------------------------------------------------------------
#include "cannon_in_out.h"
#include "rs274ngc.h"
///-----------------------------------------------------------------------------
/**
 * @note static inicialization
 */
int CannonInOutClass::_active_slot = 1;
int CannonInOutClass::_flood = 0;
int CannonInOutClass::_line_number = 1;
int CannonInOutClass::_mist = 0;
double CannonInOutClass::_feed_rate = 0.0;
double CannonInOutClass::_length_unit_factor = 1;
double CannonInOutClass::_probe_position_a = 0.0;
double CannonInOutClass::_probe_position_b = 0.0;
double CannonInOutClass::_probe_position_c = 0.0;
double CannonInOutClass::_probe_position_x = 0.0;
double CannonInOutClass::_probe_position_y = 0.0;
double CannonInOutClass::_probe_position_z = 0.0;
double CannonInOutClass::_program_origin_a = 0.0;
double CannonInOutClass::_program_origin_b = 0.0;
double CannonInOutClass::_program_origin_c = 0.0;
double CannonInOutClass::_program_origin_x = 0.0;
double CannonInOutClass::_program_origin_y = 0.0;
double CannonInOutClass::_program_origin_z = 0.0;
double CannonInOutClass::_program_position_a = 0.0;
double CannonInOutClass::_program_position_b = 0.0;
double CannonInOutClass::_program_position_c = 0.0;
double CannonInOutClass::_program_position_x = 0.0;
double CannonInOutClass::_program_position_y = 0.0;
double CannonInOutClass::_program_position_z = 0.0;
double CannonInOutClass::_spindle_speed = 0.0;
double CannonInOutClass::_traverse_rate = 0.0;

CANON_DIRECTION   CannonInOutClass::_spindle_turning = CANON_STOPPED;
CANON_MOTION_MODE CannonInOutClass::_motion_mode = CANON_CONTINUOUS;
CANON_UNITS       CannonInOutClass::_length_unit_type = CANON_UNITS_MM;
CANON_PLANE       CannonInOutClass::_active_plane = CANON_PLANE_XY;

int   CannonInOutClass::_tool_max = 68;         /// Not static. Driver reads
FILE* CannonInOutClass::_outfile = nullptr;
char  CannonInOutClass::_parameter_file_name[PARAMETER_FILE_NAME_SIZE];          /// Not static.Driver writes
CANON_TOOL_TABLE CannonInOutClass::_tools[CANON_TOOL_MAX]; /// Not static. Driver writes

void CannonInOutClass::SetOutFile(FILE* file)
{
    _outfile = file;
}
///-----------------------------------------------------------------------------
/* Representation */
void CannonInOutClass::InitCanon()
{

}
///-----------------------------------------------------------------------------


void CannonInOutClass::print_nc_line_number()
{
    char text[256];
    int k;
    int m;
    rs274ngcClass::rs274ngc_line_text(text, 256);
    for(k=0;((k < 256) && ((text[k]=='\t')||(text[k]==' ')||(text[k]=='/')));k++);
    if((k < 256)&&((text[k]=='n')||(text[k]=='N')))
    {
        fputc('N',_outfile);
        for(k++, m=0;((k < 256)&&(text[k] >= '0')&&(text[k] <= '9'));k++, m++)
            fputc(text[k],_outfile);
        for(;m < 6; m++)
            fputc(' ',_outfile);
    }
    else
        if(k < 256)
            fprintf(_outfile,"N..... ");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::PRINT0(const char* control)
{
    fprintf(_outfile,"%5d ",_line_number++);
    print_nc_line_number();
    fprintf(_outfile,control);
}
///-----------------------------------------------------------------------------
void CannonInOutClass::PRINT1(const char* control,const char* arg1)
{
    fprintf(_outfile,"%5d ",_line_number++);
    print_nc_line_number();
    fprintf(_outfile,control,arg1);
}
///-----------------------------------------------------------------------------
/* Offset the origin to the point with absolute coordinates x, y, z,
a, b, and c. Values of x, y, z, a, b, and c are real numbers. The units
are whatever length units are being used at the time this command is
given. */
void CannonInOutClass::SetOriginOffsets(double x,double y,double z,double a,double b,double c)
{
    fprintf(_outfile, "%5d ", _line_number++);
    print_nc_line_number();
    fprintf(_outfile, "SET_ORIGIN_OFFSETS(%.4f, %.4f, %.4f"
           ", %.4f"", %.4f"", %.4f"
           ")\n",x,y,z,a,b,c
           );
    _program_position_x = _program_position_x + _program_origin_x - x;
    _program_position_y = _program_position_y + _program_origin_y - y;
    _program_position_z = _program_position_z + _program_origin_z - z;
    _program_position_a = _program_position_a + _program_origin_a - a;
    _program_position_b = _program_position_b + _program_origin_b - b;
    _program_position_c = _program_position_c + _program_origin_c - c;
    _program_origin_x = x;
    _program_origin_y = y;
    _program_origin_z = z;
    _program_origin_a = a;
    _program_origin_b = b;
    _program_origin_c = c;
}
///-----------------------------------------------------------------------------
/* Use the specified units for length. Conceptually, the units must
be either inches or millimeters. */
void CannonInOutClass::UseLengthUnits(CANON_UNITS in_unit)
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
/* Use the plane designated by selected_plane as the selected plane.
   Conceptually, the selected_plane must be the XY-plane, the XZ-plane, or
   the YZ-plane. */
void CannonInOutClass::SelectPlane(CANON_PLANE in_plane)
{
    PRINT1("SELECT_PLANE(CANON_PLANE_%s)\n",
       ((in_plane==CANON_PLANE_XY) ? "XY" :
            (in_plane==CANON_PLANE_YZ) ? "YZ" :
        (in_plane==CANON_PLANE_XZ) ? "XZ" : "UNKNOWN"));
    _active_plane=in_plane;
}
///-----------------------------------------------------------------------------
/**
 * @brief SetTraverseRate
 * @param rate
 * @note  Set the traverse rate that will be used when the spindle traverses.
 *        It is expected that no cutting will occur while a traverse move
 *         is being made.
 */
void CannonInOutClass::SetTraverseRate(double rate)
{
///    PRINT1("SET_TRAVERSE_RATE(%.4f)\n", rate);
    _traverse_rate = rate;
}
///-----------------------------------------------------------------------------
/*
Move at traverse rate so that at any time during the move, all axes
have covered the same proportion of their required motion. The final
XYZ position is given by x, y, and z. If there is an a-axis, its final
position is given by a_position, and similarly for the b-axis and c-axis.
A more positive value of a rotational axis is in the counterclockwise
direction.

Clockwise or counterclockwise is from the point of view of the
workpiece. If the workpiece is fastened to a turntable, the turntable
will turn clockwise (from the point of view of the machinist or anyone
else not moving with respect to the machining center) in order to make
the tool move counterclockwise from the point of view of the
workpiece.

*/
void CannonInOutClass::StraightTraverse(double x,double y,double z,double a,double b,double c)
{
    fprintf(_outfile, "%5d ", _line_number++);
    print_nc_line_number();
    fprintf(_outfile, "STRAIGHT_TRAVERSE(%.4f, %.4f, %.4f"
           ", %.4f"
           ", %.4f"
           ", %.4f"
           ")\n", x, y, z
           , a
           , b
           , c
           );
    _program_position_x = x;
    _program_position_y = y;
    _program_position_z = z;
    _program_position_a = a;
    _program_position_b = b;
    _program_position_c = c;
}
///-----------------------------------------------------------------------------
/*

SET_FEED_RATE sets the feed rate that will be used when the spindle is
told to move at the currently set feed rate. The rate is either:
1. the rate of motion of the tool tip in the workpiece coordinate system,
   which is used when the feed_reference mode is "CANON_WORKPIECE", or
2. the rate of motion of the tool tip in the XYZ axis system, ignoring
   motion of other axes, which is used when the feed_reference mode is
   "CANON_XYZ".

The units of the rate are:

1. If the feed_reference mode is CANON_WORKPIECE:
length units (inches or millimeters according to the setting of
CANON_UNITS) per minute along the programmed path as seen by the
workpiece.

2. If the feed_reference mode is CANON_XYZ:
A. For motion including one rotational axis only: degrees per minute.
B. For motion including two rotational axes only: degrees per minute
   In this case, the rate applies to the axis with the larger angle
   to cover, and the second rotational axis rotates so that it has
   always completed the same proportion of its required motion as has
   the rotational axis to which the feed rate applies.
C. For motion involving one or more of the XYZ axes (with or without
   simultaneous rotational axis motion): length units (inches or
   millimeters according to the setting of CANON_UNITS) per minute
   along the programmed XYZ path.

*/
void CannonInOutClass::SetFeedRate(double rate)
{
//    PRINT1("SET_FEED_RATE(%.4f)\n", rate);
    _feed_rate = rate;
}
///-----------------------------------------------------------------------------
/*

This sets the feed_reference mode to either CANON_WORKPIECE or
CANON_XYZ.

The CANON_WORKPIECE mode is more natural and general, since the rate
at which the tool passes through the material must be controlled for
safe and effective machining. For machines with more than the three
standard XYZ axes, however, computing the feed rate may be
time-consuming because the trajectories that result from motion in
four or more axes may be complex. Computation of path lengths when
only XYZ motion is considered is quite simple for the two standard
motion types (straight lines and helical arcs).

Some programming languages (rs274kt, in particular) use CANON_XYZ
mode. In these languages, the task of dealing with the rate at which
the tool tip passes through material is pushed back on the NC-program
generator, where the computation of path lengths is (almost always in
1995) an off-line activity where speed of calculation is not critical.

In CANON_WORKPIECE mode, some motions cannot be carried out as fast as
the programmed feed rate would require because axis motions tend to
cancel each other. For example, an arc in the YZ-plane can exactly
cancel a rotation around the A-axis, so that the location of the tool
tip with respect to the workpiece does not change at all during the
motion; in this case, the motion should take no time, which is
impossible at any finite rate of axis motion. In such cases, the axes
should be moved as fast as possible consistent with accurate
machining.

It would be possible to omit the SET_FEED_REFERENCE command from the
canonical commands and operate always in one mode or the other,
letting the interpreter issue SET_FEED_RATE commands, if necessary to
compensate if the NC language being interpreted used the other mode.

This would create two disadvantages when the feed_reference mode
assumed by the canonical commands differed from that assumed by the NC
language being interpreted:

1. The output code could have a lot of SET_FEED_RATE commands not
found in the input code; this is a relatively minor consideration.

2. If the interpreter reads a program in language which uses the
CANON_XYZ mode and writes canonical commands in the CANON_WORKPIECE
mode, both the interpreter and the executor of the output canonical
commands would have to perform a lot of complex calculations. With the
SET_FEED_REFERENCE command available, both do only simple calculations
for the same motions.

*/
void CannonInOutClass::SetFeedReference(CANON_FEED_REFERENCE reference)
{
    PRINT1("SET_FEED_REFERENCE(%s)\n",
       (reference == CANON_WORKPIECE) ? "CANON_WORKPIECE" : "CANON_XYZ");
}
///-----------------------------------------------------------------------------
/*
This sets the motion control mode to one of: CANON_EXACT_STOP,
CANON_EXACT_PATH, or CANON_CONTINUOUS.
*/
void CannonInOutClass::SetMotionControlMode(CANON_MOTION_MODE mode)
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
/* Set the radius to use when performing cutter radius compensation. */
void CannonInOutClass::SetCutterRadiusCompensation(double radius)
{
///PRINT1("SET_CUTTER_RADIUS_COMPENSATION(%.4f)\n", radius);
}
///-----------------------------------------------------------------------------
/* Conceptually, the direction must be left (meaning the cutter
stays to the left of the programmed path) or right. */
void CannonInOutClass::StartCutterRadiusCompensation(int side)
{
    PRINT1("START_CUTTER_RADIUS_COMPENSATION(%s)\n",
        (side==CANON_SIDE_LEFT)  ? "LEFT"  :
        (side==CANON_SIDE_RIGHT) ? "RIGHT" : "UNKNOWN");
}
///-----------------------------------------------------------------------------
/* Do not apply cutter radius compensation when executing spindle
translation commands. */
void CannonInOutClass::StopCutterRadiusCompensation()
{
PRINT0 ("STOP_CUTTER_RADIUS_COMPENSATION()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::StartSpeedFeedSynch()
{
PRINT0 ("START_SPEED_FEED_SYNCH()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::StopSpeedFeedSynch()
{
PRINT0 ("STOP_SPEED_FEED_SYNCH()\n");
}
///-----------------------------------------------------------------------------
/* Move in a helical arc from the current location at the existing feed
rate. The axis of the helix is parallel to the x, y, or z axis,
according to which one is perpendicular to the selected plane. The
helical arc may degenerate to a circular arc if there is no motion
parallel to the axis of the helix.

1. If the selected plane is the xy-plane:
A. first_end is the x-coordinate of the end of the arc.
B. second_end is the y-coordinate of the end of the arc.
C. first_axis is the x-coordinate of the axis (center) of the arc.
D. second_axis is the y-coordinate of the axis.
E. axis_end_point is the z-coordinate of the end of the arc.

2. If the selected plane is the yz-plane:
A. first_end is the y-coordinate of the end of the arc.
B. second_end is the z-coordinate of the end of the arc.
C. first_axis is the y-coordinate of the axis (center) of the arc.
D. second_axis is the z-coordinate of the axis.
E. axis_end_point is the x-coordinate of the end of the arc.

3. If the selected plane is the zx-plane:
A. first_end is the z-coordinate of the end of the arc.
B. second_end is the x-coordinate of the end of the arc.
C. first_axis is the z-coordinate of the axis (center) of the arc.
D. second_axis is the x-coordinate of the axis.
E. axis_end_point is the y-coordinate of the end of the arc.

If rotation is positive, the arc is traversed counterclockwise as
viewed from the positive end of the coordinate axis perpendicular to
the currently selected plane. If rotation is negative, the arc is
traversed clockwise. If rotation is 0, first_end and second_end must
be the same as the corresponding coordinates of the current point and
no arc is made (but there may be translation parallel to the axis
perpendicular to the selected plane and motion along the rotational axes).
If rotation is 1, more than 0 but not more than 360 degrees of arc
should be made. In general, if rotation is n, the amount of rotation
in the arc should be more than ([n-1] x 360) but not more than (n x
360).

The radius of the helix is determined by the distance from the current
location to the axis of helix or by the distance from the end location
to the axis of the helix. It is recommended that the executing system
verify that the two radii are the same (within some tolerance) at the
beginning of executing this function.

While the XYZ motion is going on, move the rotational axes so that
they have always covered the same proportion of their total motion as
a point moving along the arc has of its total motion.

*/

void CannonInOutClass::ArcFeed(
    double first_end,
    double second_end,
    double first_axis,
    double second_axis,
    int rotation,
    double axis_end_point,
    double a,
    double b,
    double c
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
/* Move at existing feed rate so that at any time during the move,
all axes have covered the same proportion of their required motion.
The meanings of the parameters is the same as for STRAIGHT_TRAVERSE.*/
void CannonInOutClass::StraightFeed(
 double x, double y, double z
 , double a
 , double b
 , double c
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
/* Perform a probing operation. This is a temporary addition to the
canonical machining functions and its semantics are not defined.
When the operation is finished, all axes should be back where they
started. */
void CannonInOutClass::StraightProbe(
 double x, double y, double z
 , double a
 , double b
 , double c
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
/* stop motion after current feed */
void CannonInOutClass::Stop()
{

}
///-----------------------------------------------------------------------------
/* freeze x,y,z for a time */
void CannonInOutClass::Dwell(double seconds)
{
///PRINT1("DWELL(%.4f)\n",(float)seconds);
}
///-----------------------------------------------------------------------------
/* Retract the spindle at traverse rate to the fully retracted position. */
void CannonInOutClass::SpindleRetractTraverse()
{
    PRINT0("SPINDLE_RETRACT_TRAVERSE()\n");
}
///-----------------------------------------------------------------------------
/* Turn the spindle clockwise at the currently set speed rate. If the
spindle is already turning that way, this command has no effect. */
void CannonInOutClass::StartSpindleClockwise()
{
    PRINT0("START_SPINDLE_CLOCKWISE()\n");
    _spindle_turning=((_spindle_speed==0.0) ? CANON_STOPPED :
                                         CANON_CLOCKWISE);
}
///-----------------------------------------------------------------------------
/* Turn the spindle counterclockwise at the currently set speed rate. If
the spindle is already turning that way, this command has no effect. */
void CannonInOutClass::StartSpindleCounterClockwise()
{
    PRINT0("START_SPINDLE_COUNTERCLOCKWISE()\n");
    _spindle_turning=((_spindle_speed==0.0) ? CANON_STOPPED :
                                         CANON_COUNTERCLOCKWISE);
}
///-----------------------------------------------------------------------------
/* Set the spindle speed that will be used when the spindle is turning.
This is usually given in rpm and refers to the rate of spindle
rotation. If the spindle is already turning and is at a different
speed, change to the speed given with this command. */
void CannonInOutClass::SetSpindleSpeed(double rpm)
{
////    PRINT1("SET_SPINDLE_SPEED(%.4f)\n", rpm);
    _spindle_speed = rpm;
}
///-----------------------------------------------------------------------------
/* Stop the spindle from turning. If the spindle is already stopped, this
command may be given, but it will have no effect. */
void CannonInOutClass::StopSpindleTurning()
{
    PRINT0("STOP_SPINDLE_TURNING()\n");
    _spindle_turning=CANON_STOPPED;
}
///-----------------------------------------------------------------------------
void CannonInOutClass::SpindleRetract()
{
PRINT0("SPINDLE_RETRACT()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::OrientSpindle(double orientation, CANON_DIRECTION direction)
{
///    PRINT2("ORIENT_SPINDLE(%.4f, %s)\n", orientation,
///        (direction==CANON_CLOCKWISE) ? "CANON_CLOCKWISE" :
///                                             "CANON_COUNTERCLOCKWISE");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::LockSpindleZ()
{

}
///-----------------------------------------------------------------------------
void CannonInOutClass::UseSpindleForce()
{

}
///-----------------------------------------------------------------------------
void CannonInOutClass::UseNoSpindleForce()
{
PRINT0("USE_NO_SPINDLE_FORCE()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::UseToolLengthOffset(double length)
{
///PRINT1("USE_TOOL_LENGTH_OFFSET(%.4f)\n", length);
}
///-----------------------------------------------------------------------------
/* It is assumed that each cutting tool in the machine is assigned to a
slot (intended to correspond to a slot number in a tool carousel).
This command results in the tool currently in the spindle (if any)
being returned to its slot, and the tool from the slot designated by
slot_number (if any) being inserted in the spindle.

If there is no tool in the slot designated by the slot argument, there
will be no tool in the spindle after this command is executed and no
error condition will result in the controller. Similarly, if there is
no tool in the spindle when this command is given, no tool will be
returned to the carousel and no error condition will result in the
controller, whether or not a tool was previously selected in the
program.

It is expected that when the machine tool controller is initialized,
the designated slot for a tool already in the spindle will be
established. This may be done in any manner deemed fit, including
(for, example) recording that information in a persistent, crash-proof
location so it is always available from the last time the machine was
run, or having the operator enter it. It is expected that the machine
tool controller will remember that information as long as it is
not re-initialized; in particular, it will be remembered between
programs.

For the purposes of this command, the tool includes the tool holder.

For machines which can carry out a select_tool command separately from
a change_tool command, the select_tool command must have been given
before the change_tool command, and the value of slot must be the slot
number of the selected tool. */

/* slot is slot number */
void CannonInOutClass::ChangeTool(int slot)
{
//    PRINT1("CHANGE_TOOL(%d)\n", slot);
    _active_slot=slot;
}
///-----------------------------------------------------------------------------
/* i is slot number */
void CannonInOutClass::SelectTool(int slot)
{
///PRINT1("SELECT_TOOL(%d)\n", slot);
}
///-----------------------------------------------------------------------------
/* Clamp the given axis. If the machining center does not have a clamp
for that axis, this command should result in an error condition in the
controller.

An attempt to move an axis while it is clamped should result in an
error condition in the controller. */
void CannonInOutClass::ClampAxis(CANON_AXIS axis)
{
    PRINT1("CLAMP_AXIS(%s)\n",
        (axis==CANON_AXIS_X) ? "CANON_AXIS_X" :
        (axis==CANON_AXIS_Y) ? "CANON_AXIS_Y" :
        (axis==CANON_AXIS_Z) ? "CANON_AXIS_Z" :
        (axis==CANON_AXIS_A) ? "CANON_AXIS_A" :
        (axis==CANON_AXIS_C) ? "CANON_AXIS_C" : "UNKNOWN");
}
///-----------------------------------------------------------------------------
/* This function has no physical effect. If commands are being printed or
logged, the comment command is printed or logged, including the string
which is the value of comment_text. This serves to allow formal
comments at specific locations in programs or command files. */
void CannonInOutClass::Comment(char *s)
{
PRINT1("COMMENT(\"%s\")\n", s);
}
///-----------------------------------------------------------------------------
void CannonInOutClass::DisableFeedOverride()
{
PRINT0("DISABLE_FEED_OVERRIDE()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::EnableFeedOverride()
{
PRINT0("ENABLE_FEED_OVERRIDE()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::DisableSpeedOverride()
{
PRINT0("DISABLE_SPEED_OVERRIDE()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::EnableSpeedOverride()
{
PRINT0("ENABLE_SPEED_OVERRIDE()\n");
}
///-----------------------------------------------------------------------------
/* Turn flood coolant off. */
void CannonInOutClass::FloodOff()
{
    PRINT0("FLOOD_OFF()\n");
    _flood=0;
}
///-----------------------------------------------------------------------------
/* Turn flood coolant on. */
void CannonInOutClass::FloodOn()
{
    PRINT0("FLOOD_ON()\n");
    _flood=1;
}
///-----------------------------------------------------------------------------
void CannonInOutClass::Message(char *s)
{
PRINT1("MESSAGE(\"%s\")\n", s);
}
///-----------------------------------------------------------------------------
/* Turn mist coolant off. */
void CannonInOutClass::MistOff()
{
    PRINT0("MIST_OFF()\n");
    _mist = 0;
}
///-----------------------------------------------------------------------------
/* Turn mist coolant on. */
void CannonInOutClass::MistOn()
{
    PRINT0("MIST_ON()\n");
    _mist = 1;
}
///-----------------------------------------------------------------------------
/* If the machining center has a pallet shuttle mechanism (a mechanism
which switches the position of two pallets), this command should cause
that switch to be made. If either or both of the pallets are missing,
this will not result in an error condition in the controller.

If the machining center does not have a pallet shuttle, this command
should result in an error condition in the controller. */
void CannonInOutClass::PalletShuttle()
{
PRINT0("PALLET_SHUTTLE()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::TurnProbeOff()
{
PRINT0("TURN_PROBE_OFF()\n");
}
///-----------------------------------------------------------------------------
void CannonInOutClass::TurnProbeOn()
{
PRINT0("TURN_PROBE_ON()\n");
}
///-----------------------------------------------------------------------------
/* Unclamp the given axis. If the machining center does not have a clamp
for that axis, this command should result in an error condition in the
controller. */
void CannonInOutClass::UnclampAxis(CANON_AXIS axis)
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
void CannonInOutClass::NurbKnotVector()
{

}
///-----------------------------------------------------------------------------
void CannonInOutClass::NurbControlPoint(int i, double x, double y, double z, double w )
{

}
///-----------------------------------------------------------------------------
void CannonInOutClass::NurbFeed(double sStart,double sEnd)
{

}
///-----------------------------------------------------------------------------
/* If the machining center has an optional stop switch, and it is on
when this command is read from a program, stop executing the program
at this point, but be prepared to resume with the next line of the
program. If the machining center does not have an optional stop
switch, or commands are being executed with a stop after each one
already (such as when the interpreter is being used with keyboard
input), this command has no effect. */
void CannonInOutClass::OptionalProgramStop()
{
PRINT0("OPTIONAL_PROGRAM_STOP()\n");
}
///-----------------------------------------------------------------------------
/* If a program is being read, stop executing the program and be prepared
to accept a new program or to be shut down. */
void CannonInOutClass::ProgramEnd()
{
PRINT0("PROGRAM_END()\n");
}
///-----------------------------------------------------------------------------
/* If this command is read from a program, stop executing the program at
this point, but be prepared to resume with the next line of the
program. If commands are being executed with a stop after each one
already (such as when the interpreter is being used with keyboard
input), this command has no effect. */
void CannonInOutClass::ProgramStop()
{
PRINT0("PROGRAM_STOP()\n");
}
///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
///The interpreter==not using this function
// Returns the system angular unit factor, in units / degree
double CannonInOutClass::GetExternalAngleUnitFactor()
{
  return 1;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_FEED_RATE
 * @return Returns the system feed rate
 */
double CannonInOutClass::GetExternalFeedRate()
{
    return _feed_rate;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_FLOOD
 * @return Returns the system flood coolant setting zero = off, non-zero = on
 */
int CannonInOutClass::GetExternalFlood()
{
    return _flood;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_LENGTH_UNIT_FACTOR
 * @return Returns the system length unit factor, in units per mm
 */
double CannonInOutClass::GetExternalLengthUnitFactor()
{
    return 1 / _length_unit_factor;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_LENGTH_UNIT_TYPE
 * @return Returns the system length unit type
 */
CANON_UNITS CannonInOutClass::GetExternalLengthUnitType()
{
    return _length_unit_type;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_MIST
 * @return Returns the system mist coolant setting zero = off, non-zero = on
 */
int CannonInOutClass::GetExternalMist()
{
    return _mist;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_MOTION_CONTROL_MODE
 * @return Returns the current motion control mode
 */
CANON_MOTION_MODE CannonInOutClass::GetExternalMotionControlMode()
{
    return _motion_mode;
}
///-----------------------------------------------------------------------------
// returns the current a-axis origin offset
double CannonInOutClass::GetExternalOriginA()
{
  return _program_origin_a;
}
///-----------------------------------------------------------------------------
// returns the current b-axis origin offset
double CannonInOutClass::GetExternalOriginB()
{
  return _program_origin_b;
}
///-----------------------------------------------------------------------------
// returns the current c-axis origin offset
double CannonInOutClass::GetExternalOriginC()
{
  return _program_origin_c;
}
///-----------------------------------------------------------------------------
// returns the current x-axis origin offset
double CannonInOutClass::GetExternalOriginX()
{
  return _program_origin_x;
}
///-----------------------------------------------------------------------------
// returns the current y-axis origin offset
double CannonInOutClass::GetExternalOriginY()
{
  return _program_origin_y;
}
///-----------------------------------------------------------------------------
// returns the current z-axis origin offset
double CannonInOutClass::GetExternalOriginZ()
{
  return _program_origin_z;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_PARAMETER_FILE_NAME
 * @param file_name - string: to copy file name into
 * @param max_size  - maximum number of characters to copy
 * @note  returns nothing but copies the name of the parameter file into
 *         the filename array, stopping at max_size if the name is longer
 *         An empty string may be placed in filename.
 */
void CannonInOutClass::GetExternalParameterFileName(char * file_name,int max_size)
{
    if(strlen(_parameter_file_name) < max_size)
        strcpy(file_name, _parameter_file_name);
    else
        file_name[0]=0;
}
///-----------------------------------------------------------------------------
// returns the currently active plane
CANON_PLANE CannonInOutClass::GetExternalPlane()
{
    return _active_plane;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_POSITION_A
 * @return returns the current a-axis position
 */
double CannonInOutClass::GetExternalPositionA()
{
    return _program_position_a;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_POSITION_B
 * @return returns the current b-axis position
 */
double CannonInOutClass::GetExternalPositionB()
{
    return _program_position_b;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_POSITION_C
 * @return returns the current c-axis position
 */
double CannonInOutClass::GetExternalPositionC()
{
    return _program_position_c;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_POSITION_X
 * @return returns the current x-axis position
 */
double CannonInOutClass::GetExternalPositionX()
{
    return _program_position_x;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_POSITION_Y
 * @return returns the current y-axis position
 */
double CannonInOutClass::GetExternalPositionY()
{
    return _program_position_y;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_POSITION_Z
 * @return returns the current z-axis position
 */
double CannonInOutClass::GetExternalPositionZ()
{
    return _program_position_z;
}
///-----------------------------------------------------------------------------
/* returns the a-axis position at the last probe trip. This==only valid
   once the probe command has executed to completion. */
double CannonInOutClass::GetExternalProbePositionA()
{
  return _probe_position_a;
}
///-----------------------------------------------------------------------------
/* returns the b-axis position at the last probe trip. This==only valid
   once the probe command has executed to completion. */
double CannonInOutClass::GetExternalProbePositionB()
{
  return _probe_position_b;
}
///-----------------------------------------------------------------------------
/* returns the c-axis position at the last probe trip. This==only valid
   once the probe command has executed to completion. */
double CannonInOutClass::GetExternalProbePositionC()
{
  return _probe_position_c;
}
///-----------------------------------------------------------------------------
/* returns the x-axis position at the last probe trip. This==only valid
   once the probe command has executed to completion. */
double CannonInOutClass::GetExternalProbePositionX()
{
  return _probe_position_x;
}
///-----------------------------------------------------------------------------
/* returns the y-axis position at the last probe trip. This==only valid
   once the probe command has executed to completion. */
double CannonInOutClass::GetExternalProbePositionY()
{
  return _probe_position_y;
}
///-----------------------------------------------------------------------------
/* returns the z-axis position at the last probe trip. This==only valid
   once the probe command has executed to completion. */
double CannonInOutClass::GetExternalProbePositionZ()
{
  return _probe_position_z;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_PROBE_VALUE
 * @return Returns the value for any analog non-contact probing.
 *   This==a dummy of a dummy, returning a useless value.
 *   It==not expected this will ever be called.
 */
double CannonInOutClass::GetExternalProbeValue()
{
  return 1.0;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_QUEUE_EMPTY
 * @return Returns zero if queue==not empty, non-zero if the queue==empty
 *  In the stand-alone interpreter, there==no queue, so it==always empty
 */
int CannonInOutClass::GetExternalQueueEmpty()
{
  return 1;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_SPEED
 * @return Returns the system value for spindle speed in rpm
 */
double CannonInOutClass::GetExternalSpeed()
{
  return _spindle_speed;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_SPINDLE
 * @return Returns the system value for direction of spindle turning
 */
CANON_DIRECTION CannonInOutClass::GetExternalSpindle()
{
  return _spindle_turning;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_TOOL_SLOT
 * @return Returns the system value for the carousel slot in which the tool
 *    currently in the spindle belongs. Return value zero means there==no
 *    tool in the spindle.
 */
int CannonInOutClass::GetExternalToolSlot()
{
  return _active_slot;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_TOOL_MAX
 * @return Returns maximum number of tools
 */
int CannonInOutClass::GetExternalToolMax()
{
  return _tool_max;
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_TOOL_TABLE
 * @param pocket
 * @return Returns the CANON_TOOL_TABLE structure associated with the tool
 *          in the given pocket
 */
CANON_TOOL_TABLE CannonInOutClass::GetExternalToolTable(int pocket)
{
  return _tools[pocket];
}
///-----------------------------------------------------------------------------
/**
 * @brief GET_EXTERNAL_TRAVERSE_RATE
 * @return Returns the system traverse rate
 */
double CannonInOutClass::GetExternalTraverseRate()
{
  return _traverse_rate;
}
///-----------------------------------------------------------------------------

