///-----------------------------------------------------------------------------
#ifndef COMMAND_H
#define COMMAND_H
///-----------------------------------------------------------------------------
#include <QString>
#include <QPair>
#include <QVector>
///-----------------------------------------------------------------------------
class CommandClass
{
    public:
        CommandClass();
        virtual ~CommandClass();
        QVector<QPair<int,QString>> motion_command;
};
///-----------------------------------------------------------------------------
#endif /// COMMAND_H
///-----------------------------------------------------------------------------
#if 0
stream << "SET_ORIGIN_OFFSETS" << "(" << x  << " ,"  << y  << " ,"  << z  << " ," << a  << " ," << b  << " ," << c  << ")\n";
    "USE_LENGTH_UNITS(CANON_UNITS_INCHES)\n");
    "USE_LENGTH_UNITS(CANON_UNITS_MM)\n");
    "USE_LENGTH_UNITS(UNKNOWN)\n");
"SELECT_PLANE",
   ((in_plane==CANON_PLANE_XY) ? "XY" :
        (in_plane==CANON_PLANE_YZ) ? "YZ" :
    (in_plane==CANON_PLANE_XZ) ? "XZ" : "UNKNOWN"));
"SET_TRAVERSE_RATE(%.4f)\n",&rate);
stream << "STRAIGHT_TRAVERSE" << "(" << x  << " ," << y  << " ," << z  << " ," << a  << " ," << b  << " ," << c  << ")\n";
"SET_FEED_RATE",&rate);
"SET_FEED_REFERENCE",(reference == CANON_WORKPIECE) ? "CANON_WORKPIECE" : "CANON_XYZ");
    "SET_MOTION_CONTROL_MODE(CANON_EXACT_STOP)\n");
    "SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH)\n");
    "SET_MOTION_CONTROL_MODE(CANON_CONTINUOUS)\n");
    "SET_MOTION_CONTROL_MODE(UNKNOWN)\n");
"SET_CUTTER_RADIUS_COMPENSATION(%.4f)\n",&radius);
"START_CUTTER_RADIUS_COMPENSATION(%s)\n",
    (side==CANON_SIDE_LEFT)  ? "LEFT"  :
    (side==CANON_SIDE_RIGHT) ? "RIGHT" : "UNKNOWN");
PRINT0 ("STOP_CUTTER_RADIUS_COMPENSATION()\n");
PRINT0 ("START_SPEED_FEED_SYNCH()\n");
PRINT0 ("STOP_SPEED_FEED_SYNCH()\n");
stream << "STRAIGHT_TRAVERSE"<< "("<< first_end << " ," << second_end << " ," << first_axis << " ,"<< second_axis << " ," << rotation << " ," << axis_end_point  << " ," << a << " ," << b << " ," << c << ")\n";
stream << "STRAIGHT_FEED" << "("<< x << " ,"<< y << " ,"<< z << " ,"<< a << " ,"<< b << " ,"<< c << ")\n";
stream << "STRAIGHT_PROBE" << "(" << x << " ,"<< y << " ,"<< z << " ,"<< a << " ,"<< b << " ,"<< c << ")\n";
"DWELL(%.4f)\n",&seconds);
"SPINDLE_RETRACT_TRAVERSE()\n");
"START_SPINDLE_CLOCKWISE()\n");
"START_SPINDLE_COUNTERCLOCKWISE()\n");
"SET_SPINDLE_SPEED",&rpm);
"STOP_SPINDLE_TURNING()\n");
"SPINDLE_RETRACT()\n");
"ORIENT_SPINDLE(%.4f, %s)\n",&orientation,"CANON_COUNTERCLOCKWISE");
"USE_NO_SPINDLE_FORCE()\n");
"USE_TOOL_LENGTH_OFFSET",&length);
"CHANGE_TOOL",&slot);
"SELECT_TOOL",&slot);
"CLAMP_AXISn",
"COMMENT", s);
"DISABLE_FEED_OVERRIDE()\n");
"ENABLE_FEED_OVERRIDE()\n");
"DISABLE_SPEED_OVERRIDE()\n");
"ENABLE_SPEED_OVERRIDE()\n");
"FLOOD_OFF()\n");
"FLOOD_ON()\n");
"MESSAGE(\"%s\")\n",s);
"MIST_OFF()\n");
"MIST_ON()\n");
"PALLET_SHUTTLE()\n");
"TURN_PROBE_OFF()\n");
"TURN_PROBE_ON()\n");
"UNCLAMP_AXIS(%s)\n",
"OPTIONAL_PROGRAM_STOP()\n");
"PROGRAM_END()\n");
"PROGRAM_STOP()\n");

#endif
