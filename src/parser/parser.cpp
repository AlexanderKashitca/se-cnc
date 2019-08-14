///-----------------------------------------------------------------------------
#include "parser.h"
///-----------------------------------------------------------------------------
#include <QDebug>
///-----------------------------------------------------------------------------
using namespace PARSER_SPACE;
///-----------------------------------------------------------------------------
ParserDataClass::ParserDataClass()
{
    _command  = new CommandClass;
    _file_in  = new QFile;
    _line.clear();
    _arg_int.clear();
    _arg_double.clear();
    _arg_string.clear();
    _debug = false;
}
///-----------------------------------------------------------------------------
ParserDataClass::~ParserDataClass()
{
    delete _command;
    delete _file_in;
}
///-----------------------------------------------------------------------------
void ParserDataClass::setDebug(bool enable)
{
    _debug = enable;
}
///-----------------------------------------------------------------------------
bool ParserDataClass::getDebug()
{
    return(_debug);
}
///-----------------------------------------------------------------------------
void ParserDataClass::close()
{
    _file_in->close();
}
///-----------------------------------------------------------------------------
PARESER_STATE ParserDataClass::open(QString file_path,QString file_name)
{
    QString file_name_in;

    _filename = file_name;
    file_name_in.clear();
    file_name_in.append(file_path);
    file_name_in.append("/");
    file_name_in.append(file_name);

    if(!QFile::exists(file_name_in))
    {
        return(PARESER_FILE_NOT_EXIST);
    }
    _file_in->setFileName(file_name_in);
    if(!_file_in->open(QIODevice::ReadOnly))
    {
        return(PARESER_FILE_NOT_OPEN);
    }
    return(PARESER_OK);
}
///-----------------------------------------------------------------------------
PARESER_STATE ParserDataClass::execut(QString file_path,QString file_name,EXEC_MODE mode)
{
    int     index(-1);
    PARESER_STATE state;
    QString command;
    QTextStream in(_file_in);

    state = open(file_path,file_name);
    if(state != PARESER_OK)
        return(state);

    while(!in.atEnd())
    {
        _line.clear();
        _line = in.readLine();
        if(_debug)
            qDebug() << _line << endl;
        /// find command in the line
        index = -1;
        for(int i = 0;i < _command->motion_command.size();i++)
        {
            command = _command->motion_command.at(i).second;
            if(_line.contains(command))
            {
                index = _command->motion_command.at(i).first;
                break;
            }
        }
        if(index >= 0)
        { /// command finded
            /// get arguments from the command
            switch(index)
            {
                case 0  : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "SET_ORIGIN_OFFSETS";
                case 1  : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "STRAIGHT_TRAVERSE";
                case 2  : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "STRAIGHT_FEED";
                case 3  : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "STRAIGHT_PROBE";
                case 4  : getArguments(ARGUMENT_TYPE_INT);    break; /// "USE_LENGTH_UNITS";
                case 5  : getArguments(ARGUMENT_TYPE_INT);    break; /// "SET_MOTION_CONTROL_MODE";
                case 6  : getArguments(ARGUMENT_TYPE_INT);    break; /// "SELECT_PLANE";
                case 7  : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "SET_TRAVERSE_RATE";
                case 8  : getArguments(ARGUMENT_TYPE_INT);    break; /// "SET_FEED_RATE";
                case 9  : getArguments(ARGUMENT_TYPE_INT);    break; /// "SET_FEED_REFERENCE";
                case 10 : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "SET_CUTTER_RADIUS_COMPENSATION";
                case 11 : getArguments(ARGUMENT_TYPE_INT);    break; /// "START_CUTTER_RADIUS_COMPENSATION";
                case 12 : getArguments(ARGUMENT_TYPE_NO);     break; /// "STOP_CUTTER_RADIUS_COMPENSATION";
                case 13 : getArguments(ARGUMENT_TYPE_NO);     break; /// "START_SPEED_FEED_SYNCH";
                case 14 : getArguments(ARGUMENT_TYPE_NO);     break; /// "STOP_SPEED_FEED_SYNCH";
                case 15 : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "DWELL";
                case 16 : getArguments(ARGUMENT_TYPE_NO);     break; /// "SPINDLE_RETRACT_TRAVERSE";
                case 17 : getArguments(ARGUMENT_TYPE_NO);     break; /// "START_SPINDLE_CLOCKWISE";
                case 18 : getArguments(ARGUMENT_TYPE_NO);     break; /// "START_SPINDLE_COUNTERCLOCKWISE";
                case 19 : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "SET_SPINDLE_SPEED";
                case 20 : getArguments(ARGUMENT_TYPE_NO);     break; /// "STOP_SPINDLE_TURNING";
                case 21 : getArguments(ARGUMENT_TYPE_NO);     break; /// "SPINDLE_RETRACT";
                case 22 : getArguments(ARGUMENT_TYPE_ORIENT); break; /// "ORIENT_SPINDLE";
                case 23 : getArguments(ARGUMENT_TYPE_NO);     break; /// "USE_NO_SPINDLE_FORCE";
                case 24 : getArguments(ARGUMENT_TYPE_DOUBLE); break; /// "USE_TOOL_LENGTH_OFFSET";
                case 25 : getArguments(ARGUMENT_TYPE_INT);    break; /// "CHANGE_TOOL";
                case 26 : getArguments(ARGUMENT_TYPE_INT);    break; /// "SELECT_TOOL";
                case 27 : getArguments(ARGUMENT_TYPE_INT);    break; /// "CLAMP_AXIS";
                case 28 : getArguments(ARGUMENT_TYPE_STRING); break; /// "COMMENT";
                case 29 : getArguments(ARGUMENT_TYPE_NO);     break; /// "DISABLE_FEED_OVERRIDE";
                case 30 : getArguments(ARGUMENT_TYPE_NO);     break; /// "ENABLE_FEED_OVERRIDE";
                case 31 : getArguments(ARGUMENT_TYPE_NO);     break; /// "DISABLE_SPEED_OVERRIDE";
                case 32 : getArguments(ARGUMENT_TYPE_NO);     break; /// "ENABLE_SPEED_OVERRIDE";
                case 33 : getArguments(ARGUMENT_TYPE_NO);     break; /// "FLOOD_OFF";
                case 34 : getArguments(ARGUMENT_TYPE_NO);     break; /// "FLOOD_ON";
                case 35 : getArguments(ARGUMENT_TYPE_STRING); break; /// "MESSAGE";
                case 36 : getArguments(ARGUMENT_TYPE_NO);     break; /// "MIST_OFF";
                case 37 : getArguments(ARGUMENT_TYPE_NO);     break; /// "MIST_ON";
                case 38 : getArguments(ARGUMENT_TYPE_NO);     break; /// "PALLET_SHUTTLE";
                case 39 : getArguments(ARGUMENT_TYPE_NO);     break; /// "TURN_PROBE_OFF";
                case 40 : getArguments(ARGUMENT_TYPE_NO);     break; /// "TURN_PROBE_ON";
                case 41 : getArguments(ARGUMENT_TYPE_INT);    break; /// "UNCLAMP_AXIS";
                case 42 : getArguments(ARGUMENT_TYPE_NO);     break; /// "OPTIONAL_PROGRAM_STOP";
                case 43 : getArguments(ARGUMENT_TYPE_NO);     break; /// "PROGRAM_END";
                case 44 : getArguments(ARGUMENT_TYPE_NO);     break; /// "PROGRAM_STOP";
                case 45 : getArguments(ARGUMENT_TYPE_ARC);    break; /// "ARC_FEED";
            }
        }
    }
    close();
    return(state);
}
///-----------------------------------------------------------------------------
void ParserDataClass::getArguments(ARGUMENT_TYPE type)
{
    int end;
    int begin;
    int count(0);
    QString data;

    data.clear();
    _arg_int.clear();
    _arg_double.clear();
    _arg_string.clear();

    if(type == ARGUMENT_TYPE_NO)
        return;

    begin = _line.indexOf("(");
    end   = _line.indexOf(")");
    for(int index = begin + 1;index <= end;index++)
    {
        if((_line.at(index) != ',') && (_line.at(index) != ')'))
        {
            data.append(_line.at(index));
        }
        else
        {
            switch(type)
            {
                case ARGUMENT_TYPE_INT    : _arg_int.push_back(data.toInt());       break;
                case ARGUMENT_TYPE_DOUBLE : _arg_double.push_back(data.toDouble()); break;
                case ARGUMENT_TYPE_STRING : _arg_string.push_back(data);            break;
                case ARGUMENT_TYPE_ARC    :
                    switch(count)
                    {
                        case 0 : _arg_double.push_back(data.toDouble()); break;
                        case 1 : _arg_double.push_back(data.toDouble()); break;
                        case 2 : _arg_double.push_back(data.toDouble()); break;
                        case 3 : _arg_double.push_back(data.toDouble()); break;
                        case 4 : _arg_int.push_back(data.toInt());       break;
                        case 5 : _arg_double.push_back(data.toDouble()); break;
                        case 6 : _arg_double.push_back(data.toDouble()); break;
                        case 7 : _arg_double.push_back(data.toDouble()); break;
                        case 8 : _arg_double.push_back(data.toDouble()); break;
                    }
                    count++;
                    break;
                case ARGUMENT_TYPE_ORIENT :
                    switch(count)
                    {
                        case 0 : _arg_double.push_back(data.toDouble()); break;
                        case 1 : _arg_int.push_back(data.toInt());       break;
                    }
                    count++;
                    break;
                default : break;
            }
            data.clear();
        }
    }
}
///-----------------------------------------------------------------------------
