///-----------------------------------------------------------------------------
#include "parser.h"
///-----------------------------------------------------------------------------
#include <QDebug>
///-----------------------------------------------------------------------------
ParserDataClass::ParserDataClass()
{
    _command  = new CommandClass;
    _file_in  = new QFile;
    _line.clear();
    _arg_int.clear();
    _arg_double.clear();
    _arg_string.clear();
}
///-----------------------------------------------------------------------------
ParserDataClass::~ParserDataClass()
{
    delete _command;
    delete _file_in;
}
///-----------------------------------------------------------------------------
void ParserDataClass::close()
{
    _file_in->close();
}
///-----------------------------------------------------------------------------
bool ParserDataClass::open(QString file_path,QString file_name)
{
    QString file_name_in;

    _filename = file_name;
    file_name_in.clear();
    file_name_in.append(file_path);
    file_name_in.append("/");
    file_name_in.append(file_name);

    if(!QFile::exists(file_name_in))
    {
        return(false);
    }
    _file_in->setFileName(file_name_in);
    if(!_file_in->open(QIODevice::ReadOnly))
    {
        return(false);
    }
    return(true);
}
///-----------------------------------------------------------------------------
bool ParserDataClass::parse()
{
    int     index(-1);
    QString command;
    QTextStream in(_file_in);

    while(!in.atEnd())
    {
        _line.clear();
        _line = in.readLine();
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
                case 0  : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "SET_ORIGIN_OFFSETS";
                case 1  : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "STRAIGHT_TRAVERSE";
                case 2  : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "STRAIGHT_FEED";
                case 3  : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "STRAIGHT_PROBE";
                case 4  : GetArguments(ARGUMENT_TYPE_INT);    break; /// "USE_LENGTH_UNITS";
                case 5  : GetArguments(ARGUMENT_TYPE_INT);    break; /// "SET_MOTION_CONTROL_MODE";
                case 6  : GetArguments(ARGUMENT_TYPE_INT);    break; /// "SELECT_PLANE";
                case 7  : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "SET_TRAVERSE_RATE";
                case 8  : GetArguments(ARGUMENT_TYPE_INT);    break; /// "SET_FEED_RATE";
                case 9  : GetArguments(ARGUMENT_TYPE_INT);    break; /// "SET_FEED_REFERENCE";
                case 10 : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "SET_CUTTER_RADIUS_COMPENSATION";
                case 11 : GetArguments(ARGUMENT_TYPE_INT);    break; /// "START_CUTTER_RADIUS_COMPENSATION";
                case 12 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "STOP_CUTTER_RADIUS_COMPENSATION";
                case 13 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "START_SPEED_FEED_SYNCH";
                case 14 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "STOP_SPEED_FEED_SYNCH";
                case 15 : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "DWELL";
                case 16 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "SPINDLE_RETRACT_TRAVERSE";
                case 17 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "START_SPINDLE_CLOCKWISE";
                case 18 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "START_SPINDLE_COUNTERCLOCKWISE";
                case 19 : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "SET_SPINDLE_SPEED";
                case 20 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "STOP_SPINDLE_TURNING";
                case 21 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "SPINDLE_RETRACT";
                case 22 : GetArguments(ARGUMENT_TYPE_ORIENT); break; /// "ORIENT_SPINDLE";
                case 23 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "USE_NO_SPINDLE_FORCE";
                case 24 : GetArguments(ARGUMENT_TYPE_DOUBLE); break; /// "USE_TOOL_LENGTH_OFFSET";
                case 25 : GetArguments(ARGUMENT_TYPE_INT);    break; /// "CHANGE_TOOL";
                case 26 : GetArguments(ARGUMENT_TYPE_INT);    break; /// "SELECT_TOOL";
                case 27 : GetArguments(ARGUMENT_TYPE_INT);    break; /// "CLAMP_AXIS";
                case 28 : GetArguments(ARGUMENT_TYPE_STRING); break; /// "COMMENT";
                case 29 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "DISABLE_FEED_OVERRIDE";
                case 30 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "ENABLE_FEED_OVERRIDE";
                case 31 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "DISABLE_SPEED_OVERRIDE";
                case 32 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "ENABLE_SPEED_OVERRIDE";
                case 33 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "FLOOD_OFF";
                case 34 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "FLOOD_ON";
                case 35 : GetArguments(ARGUMENT_TYPE_STRING); break; /// "MESSAGE";
                case 36 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "MIST_OFF";
                case 37 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "MIST_ON";
                case 38 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "PALLET_SHUTTLE";
                case 39 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "TURN_PROBE_OFF";
                case 40 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "TURN_PROBE_ON";
                case 41 : GetArguments(ARGUMENT_TYPE_INT);    break; /// "UNCLAMP_AXIS";
                case 42 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "OPTIONAL_PROGRAM_STOP";
                case 43 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "PROGRAM_END";
                case 44 : GetArguments(ARGUMENT_TYPE_NO);     break; /// "PROGRAM_STOP";
                case 45 : GetArguments(ARGUMENT_TYPE_ARC);    break; /// "ARC_FEED";
            }
        }
    }
    return(true);
}
///-----------------------------------------------------------------------------
void ParserDataClass::GetArguments(ARGUMENT_TYPE type)
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
