///-----------------------------------------------------------------------------
#ifndef COMMAND_H
#define COMMAND_H
///-----------------------------------------------------------------------------
#include <QString>
///-----------------------------------------------------------------------------
int convertToIndex(QString);
int commandGeneral(int);
///-----------------------------------------------------------------------------
const QString
stream << "SET_ORIGIN_OFFSETS";
stream << "STRAIGHT_TRAVERSE";
stream << "STRAIGHT_TRAVERSE";
stream << "STRAIGHT_FEED";
stream << "STRAIGHT_PROBE";

PRINT0("USE_LENGTH_UNITS;
PRINT0("SET_MOTION_CONTROL_MODE";


PRINT1("SELECT_PLANE";
PRINT1("SET_TRAVERSE_RATE";

PRINT1("SET_FEED_RATE";
PRINT1("SET_FEED_REFERENCE";

PRINT1("SET_CUTTER_RADIUS_COMPENSATION";
PRINT1("START_CUTTER_RADIUS_COMPENSATION";
PRINT0 ("STOP_CUTTER_RADIUS_COMPENSATION";
PRINT0 ("START_SPEED_FEED_SYNCH";
PRINT0 ("STOP_SPEED_FEED_SYNCH";

PRINT1("DWELL";
PRINT0("SPINDLE_RETRACT_TRAVERSE";
PRINT0("START_SPINDLE_CLOCKWISE";
PRINT0("START_SPINDLE_COUNTERCLOCKWISE";
PRINT1("SET_SPINDLE_SPEED";
PRINT0("STOP_SPINDLE_TURNING";
PRINT0("SPINDLE_RETRACT";
PRINT2("ORIENT_SPINDLE";
PRINT0("USE_NO_SPINDLE_FORCE";
PRINT1("USE_TOOL_LENGTH_OFFSET";
PRINT1("CHANGE_TOOL";
PRINT1("SELECT_TOOL";
PRINT1("CLAMP_AXIS";
PRINT1("COMMENT";
PRINT0("DISABLE_FEED_OVERRIDE";
PRINT0("ENABLE_FEED_OVERRIDE";
PRINT0("DISABLE_SPEED_OVERRIDE";
PRINT0("ENABLE_SPEED_OVERRIDE";
PRINT0("FLOOD_OFF";
PRINT0("FLOOD_ON";
PRINT1("MESSAGE";
PRINT0("MIST_OFF";
PRINT0("MIST_ON";
PRINT0("PALLET_SHUTTLE";
PRINT0("TURN_PROBE_OFF";
PRINT0("TURN_PROBE_ON";
PRINT1("UNCLAMP_AXIS";
PRINT0("OPTIONAL_PROGRAM_STOP";
PRINT0("PROGRAM_END";
PRINT0("PROGRAM_STOP";

///-----------------------------------------------------------------------------
#endif /// COMMAND_H
///-----------------------------------------------------------------------------
#if 0
stream << "SET_ORIGIN_OFFSETS" << "(" << x  << " ,"  << y  << " ,"  << z  << " ," << a  << " ," << b  << " ," << c  << ")\n";
    PRINT0("USE_LENGTH_UNITS(CANON_UNITS_INCHES)\n");
    PRINT0("USE_LENGTH_UNITS(CANON_UNITS_MM)\n");
    PRINT0("USE_LENGTH_UNITS(UNKNOWN)\n");
PRINT1("SELECT_PLANE",
   ((in_plane==CANON_PLANE_XY) ? "XY" :
        (in_plane==CANON_PLANE_YZ) ? "YZ" :
    (in_plane==CANON_PLANE_XZ) ? "XZ" : "UNKNOWN"));
PRINT1("SET_TRAVERSE_RATE(%.4f)\n",&rate);
stream << "STRAIGHT_TRAVERSE" << "(" << x  << " ," << y  << " ," << z  << " ," << a  << " ," << b  << " ," << c  << ")\n";
PRINT1("SET_FEED_RATE",&rate);
PRINT1("SET_FEED_REFERENCE",(reference == CANON_WORKPIECE) ? "CANON_WORKPIECE" : "CANON_XYZ");
    PRINT0("SET_MOTION_CONTROL_MODE(CANON_EXACT_STOP)\n");
    PRINT0("SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH)\n");
    PRINT0("SET_MOTION_CONTROL_MODE(CANON_CONTINUOUS)\n");
    PRINT0("SET_MOTION_CONTROL_MODE(UNKNOWN)\n");
PRINT1("SET_CUTTER_RADIUS_COMPENSATION(%.4f)\n",&radius);
PRINT1("START_CUTTER_RADIUS_COMPENSATION(%s)\n",
    (side==CANON_SIDE_LEFT)  ? "LEFT"  :
    (side==CANON_SIDE_RIGHT) ? "RIGHT" : "UNKNOWN");
PRINT0 ("STOP_CUTTER_RADIUS_COMPENSATION()\n");
PRINT0 ("START_SPEED_FEED_SYNCH()\n");
PRINT0 ("STOP_SPEED_FEED_SYNCH()\n");
stream << "STRAIGHT_TRAVERSE"<< "("<< first_end << " ," << second_end << " ," << first_axis << " ,"<< second_axis << " ," << rotation << " ," << axis_end_point  << " ," << a << " ," << b << " ," << c << ")\n";
stream << "STRAIGHT_FEED" << "("<< x << " ,"<< y << " ,"<< z << " ,"<< a << " ,"<< b << " ,"<< c << ")\n";
stream << "STRAIGHT_PROBE" << "(" << x << " ,"<< y << " ,"<< z << " ,"<< a << " ,"<< b << " ,"<< c << ")\n";
PRINT1("DWELL(%.4f)\n",&seconds);
PRINT0("SPINDLE_RETRACT_TRAVERSE()\n");
PRINT0("START_SPINDLE_CLOCKWISE()\n");
PRINT0("START_SPINDLE_COUNTERCLOCKWISE()\n");
PRINT1("SET_SPINDLE_SPEED",&rpm);
PRINT0("STOP_SPINDLE_TURNING()\n");
PRINT0("SPINDLE_RETRACT()\n");
    PRINT2("ORIENT_SPINDLE(%.4f, %s)\n",&orientation,"CANON_CLOCKWISE");
    PRINT2("ORIENT_SPINDLE(%.4f, %s)\n",&orientation,"CANON_COUNTERCLOCKWISE");
PRINT0("USE_NO_SPINDLE_FORCE()\n");
PRINT1("USE_TOOL_LENGTH_OFFSET",&length);
PRINT1("CHANGE_TOOL",&slot);
PRINT1("SELECT_TOOL",&slot);
PRINT1("CLAMP_AXISn",
PRINT1("COMMENT", s);
PRINT0("DISABLE_FEED_OVERRIDE()\n");
PRINT0("ENABLE_FEED_OVERRIDE()\n");
PRINT0("DISABLE_SPEED_OVERRIDE()\n");
PRINT0("ENABLE_SPEED_OVERRIDE()\n");
PRINT0("FLOOD_OFF()\n");
PRINT0("FLOOD_ON()\n");
PRINT1("MESSAGE(\"%s\")\n",s);
PRINT0("MIST_OFF()\n");
PRINT0("MIST_ON()\n");
PRINT0("PALLET_SHUTTLE()\n");
PRINT0("TURN_PROBE_OFF()\n");
PRINT0("TURN_PROBE_ON()\n");
PRINT1("UNCLAMP_AXIS(%s)\n",
PRINT0("OPTIONAL_PROGRAM_STOP()\n");
PRINT0("PROGRAM_END()\n");
PRINT0("PROGRAM_STOP()\n");

#endif
