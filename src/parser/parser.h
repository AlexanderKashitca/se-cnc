///-----------------------------------------------------------------------------
#ifndef PARSER_H
#define PARSER_H
///-----------------------------------------------------------------------------
#include <QFile>
#include <QTextStream>
///-----------------------------------------------------------------------------
#include "command.h"
///-----------------------------------------------------------------------------

typedef enum
{
    ARGUMENT_TYPE_NO     = 0,
    ARGUMENT_TYPE_INT    = 1,
    ARGUMENT_TYPE_DOUBLE = 2,
    ARGUMENT_TYPE_STRING = 3,
    ARGUMENT_TYPE_ARC    = 4,
    ARGUMENT_TYPE_ORIENT = 5
}ARGUMENT_TYPE;
///-----------------------------------------------------------------------------
class ParserDataClass
{
    private :
        QFile*  _file_in;
        QString _filename;
        CommandClass* _command;

        QString         _line;
        QVector<double> _arg_double;
        QVector<int>    _arg_int;
        QString         _arg_string;

        void GetArguments(ARGUMENT_TYPE type);

    public :
        ParserDataClass();
        ~ParserDataClass();
        ParserDataClass(const ParserDataClass &);


        bool open(QString file_path,QString file_name);
        void close();
        bool parse();

};
///-----------------------------------------------------------------------------
#endif /// PARSERCLDATACLASS_H
///-----------------------------------------------------------------------------
//// void InitCanon();
//// void SetOriginOffsets(double x,double y,double z,double a,double b,double c);
//// void UseLengthUnits(CANON_UNITS in_unit);
//// void SelectPlane(CANON_PLANE in_plane);
//// void SetTraverseRate(double rate);
//// void StraightTraverse(double x,double y,double z,double a_position,double b_position,double c_position);
//// void SetFeedRate(double rate);
//// void SetFeedReference(CANON_FEED_REFERENCE reference);
//// void SetMotionControlMode(CANON_MOTION_MODE mode);
//// void SetCutterRadiusCompensation(double radius);
//// void StartCutterRadiusCompensation(int direction);
//// void StopCutterRadiusCompensation();
//// void StartSpeedFeedSynch();
//// void StopSpeedFeedSynch();
//// void ArcFeed(double first_end,double second_end,double first_axis, double second_axis,int rotation,double axis_end_point,double a_position,double b_position,double c_position);
//// void StraightFeed(double x,double y,double z,double a_position,double b_position,double c_position);
//// void StraightProbe(double x,double y,double z,double a_position,double b_position,double c_position);
//// void Stop();
//// void Dwell(double seconds);
//// void SpindleRetractTraverse();
//// void StartSpindleClockwise();
//// void StartSpindleCounterClockwise();
//// void SetSpindleSpeed(double r);
//// void StopSpindleTurning();
//// void SpindleRetract();
//// void UseNoSpindleForce();
//// void OrientSpindle(double orientation, CANON_DIRECTION direction);
//// void UseToolLengthOffset(double length);
//// void ChangeTool(int slot);
//// void SelectTool(int i);
//// void ClampAxis(CANON_AXIS axis);
//// void UnclampAxis(CANON_AXIS axis);
//// void Comment(char *s);
//// void DisableFeedOverride();
//// void EnableFeedOverride();
//// void DisableSpeedOverride();
//// void EnableSpeedOverride();
//// void FloodOff();
//// void FloodOn();
//// void Message(char *s);
//// void MistOff();
//// void MistOn();
//// void PalletShuttle();
//// void TurnProbeOff();
//// void TurnProbeOn();
//// void OptionalProgramStop();
//// void ProgramEnd();
//// void ProgramStop();


//// void NurbKnotVector(); /* double knot values, -1.0 signals done */
//// void NurbControlPoint(int i,double x,double y,double z,double w);
//// void NurbFeed(double sStart,double sEnd);
//// void LockSpindleZ();
//// void UseSpindleForce();
