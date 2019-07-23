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
    private:
        void print_nc_line_number();
        FILE* _outfile;

    public:
        CannonInOutClass();
        CannonInOutClass(const CannonInOutClass&);
        ~CannonInOutClass();

        /* reads world model data into the canonical interface */
        void InitCanon();
        /* Representation */
        void SetOroginOffsets(
            double x, double y, double z,
            double a, double b, double c
        );
        /* Offset the origin to the point with absolute coordinates x, y, z,
        a, b, and c. Values of x, y, z, a, b, and c are real numbers. The units
        are whatever length units are being used at the time this command is
        given. */
        void UseLengthUnits(CANON_UNITS in_unit);
        /* Use the specified units for length. Conceptually, the units must
        be either inches or millimeters. */
        void SelectPlane(CANON_PLANE in_plane);
        /* Use the plane designated by selected_plane as the selected plane.
           Conceptually, the selected_plane must be the XY-plane, the XZ-plane, or
           the YZ-plane. */
        /* Free Space Motion */
        void SetTraverseRate(double rate);
        /* Set the traverse rate that will be used when the spindle traverses. It
        is expected that no cutting will occur while a traverse move is being
        made. */
        void StaightTraverse(
         double x, double y, double z
         , double a_position
         , double b_position
         , double c_position
        );
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

        /* Machining Attributes */

         void SetFeedRate(double rate);

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

         void SetFeedReference(CANON_FEED_REFERENCE reference);

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

         void SetMotionControlMode(CANON_MOTION_MODE mode);

        /*
        This sets the motion control mode to one of: CANON_EXACT_STOP,
        CANON_EXACT_PATH, or CANON_CONTINUOUS.
        */

         void SetCutterRadiusCompensation(double radius);

        /* Set the radius to use when performing cutter radius compensation. */

         void StartCutterRadiusCompensation(int direction);

        /* Conceptually, the direction must be left (meaning the cutter
        stays to the left of the programmed path) or right. */

         void StopCutterRadiusCompensation();

        /* Do not apply cutter radius compensation when executing spindle
        translation commands. */

         void StartSpeedFeedSynch();
         void StopSpeedFeedSynch();

        /* Machining Functions */

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

         void StraightFeed(
         double x, double y, double z
         , double a_position
         , double b_position
         , double c_position
        );

        /* Move at existing feed rate so that at any time during the move,
        all axes have covered the same proportion of their required motion.
        The meanings of the parameters is the same as for STRAIGHT_TRAVERSE.*/

         void StraightProbe(
         double x, double y, double z
         , double a_position
         , double b_position
         , double c_position
        );

        /* Perform a probing operation. This is a temporary addition to the
        canonical machining functions and its semantics are not defined.
        When the operation is finished, all axes should be back where they
        started. */

         void Stop();

        /* stop motion after current feed */

         void Dwell(double seconds);

        /* freeze x,y,z for a time */

        /* Spindle Functions */

         void SpindleRetractTraverse();

        /* Retract the spindle at traverse rate to the fully retracted position. */

         void StartSpindleClockwise();

        /* Turn the spindle clockwise at the currently set speed rate. If the
        spindle is already turning that way, this command has no effect. */

         void StartSpindleCounterClockwise();

        /* Turn the spindle counterclockwise at the currently set speed rate. If
        the spindle is already turning that way, this command has no effect. */

         void SetSpindleSpeed(double r);

        /* Set the spindle speed that will be used when the spindle is turning.
        This is usually given in rpm and refers to the rate of spindle
        rotation. If the spindle is already turning and is at a different
        speed, change to the speed given with this command. */

         void StopSpindleTurning();

        /* Stop the spindle from turning. If the spindle is already stopped, this
        command may be given, but it will have no effect. */

         void SpindleRetract();
         void OrientSpindle(double orientation, CANON_DIRECTION direction);
         void LockSpindleZ();
         void UseSpindleForce();
         void UseNoSpindleForce();

        /* Tool Functions */
         void UseToolLengthOffset(double length);

         void ChangeTool(int slot); /* slot is slot number */

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

         void SelectTool(int i); /* i is slot number */

        /* Miscellaneous Functions */

         void ClampAxis(CANON_AXIS axis);

        /* Clamp the given axis. If the machining center does not have a clamp
        for that axis, this command should result in an error condition in the
        controller.

        An attempt to move an axis while it is clamped should result in an
        error condition in the controller. */

         void Comment(char *s);

        /* This function has no physical effect. If commands are being printed or
        logged, the comment command is printed or logged, including the string
        which is the value of comment_text. This serves to allow formal
        comments at specific locations in programs or command files. */

         void DisableFeedOverride();
         void EnableFeedOverride();
         void DisableSpeedOverride();
         void EnableSpeedOverride();
         void FloodOff();
        /* Turn flood coolant off. */
         void FloodOn();
        /* Turn flood coolant on. */

        void Message(char *s);

        void MistOff();
        /* Turn mist coolant off. */

        void MistOn();
        /* Turn mist coolant on. */

        void PalletShuttle();

        /* If the machining center has a pallet shuttle mechanism (a mechanism
        which switches the position of two pallets), this command should cause
        that switch to be made. If either or both of the pallets are missing,
        this will not result in an error condition in the controller.

        If the machining center does not have a pallet shuttle, this command
        should result in an error condition in the controller. */

        void TurnProbeOff();
        void TurnProbeOn();

        void UnclampAxis(CANON_AXIS axis);

        /* Unclamp the given axis. If the machining center does not have a clamp
        for that axis, this command should result in an error condition in the
        controller. */

        /* NURB Functions */
        void NurbKnotVector(); /* double knot values, -1.0 signals done */
        void NurbControlPoint(int i, double x, double y, double z, double w );
        void NurbFeed(double sStart, double sEnd);

        /* Program Functions */
        void OptionalProgramStop();

        /* If the machining center has an optional stop switch, and it is on
        when this command is read from a program, stop executing the program
        at this point, but be prepared to resume with the next line of the
        program. If the machining center does not have an optional stop
        switch, or commands are being executed with a stop after each one
        already (such as when the interpreter is being used with keyboard
        input), this command has no effect. */

        void ProgramEnd();
        /* If a program is being read, stop executing the program and be prepared
        to accept a new program or to be shut down. */

        void ProgramStop();
        /* If this command is read from a program, stop executing the program at
        this point, but be prepared to resume with the next line of the
        program. If commands are being executed with a stop after each one
        already (such as when the interpreter is being used with keyboard
        input), this command has no effect. */



};
///-----------------------------------------------------------------------------
#endif /// CANNON_IN_OUT_H
///-----------------------------------------------------------------------------
