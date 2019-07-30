///-----------------------------------------------------------------------------
#include "command.h"
///-----------------------------------------------------------------------------
CommandClass::CommandClass()
{
    QPair<int,QString> pair;
    motion_command.clear();
    ///-------------------------------------------------------------------------
    pair.first = 0;
    pair.second = "SET_ORIGIN_OFFSETS";
    motion_command.push_back(pair);
    pair.first = 1;
    pair.second = "STRAIGHT_TRAVERSE";
    motion_command.push_back(pair);
    pair.first = 2;
    pair.second = "STRAIGHT_FEED";
    motion_command.push_back(pair);
    pair.first = 3;
    pair.second = "STRAIGHT_PROBE";
    motion_command.push_back(pair);
    pair.first = 4;
    pair.second = "USE_LENGTH_UNITS";
    motion_command.push_back(pair);
    pair.first = 5;
    pair.second = "SET_MOTION_CONTROL_MODE";
    motion_command.push_back(pair);
    pair.first = 6;
    pair.second = "SELECT_PLANE";
    motion_command.push_back(pair);
    pair.first = 7;
    pair.second = "SET_TRAVERSE_RATE";
    motion_command.push_back(pair);
    pair.first = 8;
    pair.second = "SET_FEED_RATE";
    motion_command.push_back(pair);
    pair.first = 9;
    pair.second = "SET_FEED_REFERENCE";
    motion_command.push_back(pair);
    ///-------------------------------------------------------------------------
    pair.first = 10;
    pair.second = "SET_CUTTER_RADIUS_COMPENSATION";
    motion_command.push_back(pair);
    pair.first = 11;
    pair.second = "START_CUTTER_RADIUS_COMPENSATION";
    motion_command.push_back(pair);
    pair.first = 12;
    pair.second = "STOP_CUTTER_RADIUS_COMPENSATION";
    motion_command.push_back(pair);
    pair.first = 13;
    pair.second = "START_SPEED_FEED_SYNCH";
    motion_command.push_back(pair);
    pair.first = 14;
    pair.second = "STOP_SPEED_FEED_SYNCH";
    motion_command.push_back(pair);
    pair.first = 15;
    pair.second = "DWELL";
    motion_command.push_back(pair);
    pair.first = 16;
    pair.second = "SPINDLE_RETRACT_TRAVERSE";
    motion_command.push_back(pair);
    pair.first = 17;
    pair.second = "START_SPINDLE_CLOCKWISE";
    motion_command.push_back(pair);
    pair.first = 18;
    pair.second = "START_SPINDLE_COUNTERCLOCKWISE";
    motion_command.push_back(pair);
    pair.first = 19;
    pair.second = "SET_SPINDLE_SPEED";
    motion_command.push_back(pair);
    ///-------------------------------------------------------------------------
    pair.first = 20;
    pair.second = "STOP_SPINDLE_TURNING";
    motion_command.push_back(pair);
    pair.first = 21;
    pair.second = "SPINDLE_RETRACT";
    motion_command.push_back(pair);
    pair.first = 22;
    pair.second = "ORIENT_SPINDLE";
    motion_command.push_back(pair);
    pair.first = 23;
    pair.second = "USE_NO_SPINDLE_FORCE";
    motion_command.push_back(pair);
    pair.first = 24;
    pair.second = "USE_TOOL_LENGTH_OFFSET";
    motion_command.push_back(pair);
    pair.first = 25;
    pair.second = "CHANGE_TOOL";
    motion_command.push_back(pair);
    pair.first = 26;
    pair.second = "SELECT_TOOL";
    motion_command.push_back(pair);
    pair.first = 27;
    pair.second = "CLAMP_AXIS";
    motion_command.push_back(pair);
    pair.first = 28;
    pair.second = "COMMENT";
    motion_command.push_back(pair);
    pair.first = 29;
    pair.second = "DISABLE_FEED_OVERRIDE";
    motion_command.push_back(pair);
    ///-------------------------------------------------------------------------
    pair.first = 30;
    pair.second = "ENABLE_FEED_OVERRIDE";
    motion_command.push_back(pair);
    pair.first = 31;
    pair.second = "DISABLE_SPEED_OVERRIDE";
    motion_command.push_back(pair);
    pair.first = 32;
    pair.second = "ENABLE_SPEED_OVERRIDE";
    motion_command.push_back(pair);
    pair.first = 33;
    pair.second = "FLOOD_OFF";
    motion_command.push_back(pair);
    pair.first = 34;
    pair.second = "FLOOD_ON";
    motion_command.push_back(pair);
    pair.first = 35;
    pair.second = "MESSAGE";
    motion_command.push_back(pair);
    pair.first = 36;
    pair.second = "MIST_OFF";
    motion_command.push_back(pair);
    pair.first = 37;
    pair.second = "MIST_ON";
    motion_command.push_back(pair);
    pair.first = 38;
    pair.second = "PALLET_SHUTTLE";
    motion_command.push_back(pair);
    pair.first = 39;
    pair.second = "TURN_PROBE_OFF";
    motion_command.push_back(pair);
    ///-------------------------------------------------------------------------
    pair.first = 40;
    pair.second = "TURN_PROBE_ON";
    motion_command.push_back(pair);
    pair.first = 41;
    pair.second = "UNCLAMP_AXIS";
    motion_command.push_back(pair);
    pair.first = 42;
    pair.second = "OPTIONAL_PROGRAM_STOP";
    motion_command.push_back(pair);
    pair.first = 43;
    pair.second = "PROGRAM_END";
    motion_command.push_back(pair);
    pair.first = 44;
    pair.second = "PROGRAM_STOP";
    motion_command.push_back(pair);
    pair.first = 45;
    pair.second = "ARC_FEED";
    motion_command.push_back(pair);
};
///-----------------------------------------------------------------------------
CommandClass::~CommandClass()
{

}
///-----------------------------------------------------------------------------
