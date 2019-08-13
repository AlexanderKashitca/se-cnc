///-----------------------------------------------------------------------------
#include "system.h"
///-----------------------------------------------------------------------------
using namespace SYSTEM_SPACE;
using namespace PARSER_SPACE;
using namespace INTERPRETER_SPACE;
using namespace COORD_MOTION_SPACE;
///-----------------------------------------------------------------------------
SystemClass::SystemClass()
{
    _planner = new PLANNER_SPACE::PlannerClass;
    _parser  = new PARSER_SPACE::ParserDataClass;
    _interpreter = new INTERPRETER_SPACE::InterpreterClass;
    _coordinate  = new COORD_MOTION_SPACE::CoordMotionClass;
    _debug = false;
}
///-----------------------------------------------------------------------------
SystemClass::~SystemClass()
{
    delete _planner;
    delete _parser;
    delete _interpreter;
    delete _coordinate;
}
///-----------------------------------------------------------------------------
void SystemClass::setDebug(bool enable)
{
    _debug = enable;
    _parser->setDebug(enable);
    _planner->setDebug(enable);
    _coordinate->setDebug(enable);
    _interpreter->setDebug(enable);
}
///-----------------------------------------------------------------------------
bool SystemClass::getDebug()
{
    return(_debug);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::initialization()
{
    /// initialization coordinate system
    if(_coordinate->initialization() != COORD_STATE_OK)
    {
        if(_debug)
            qDebug() << _coordinate->getLastErrorMessage();
        return(SYSTEM_FAIL);
    }
    /// initialization interpreter
    if(_interpreter->initialization(&_interpreter_settings) != INTERPRETER_OK)
    {
        if(_debug)
            qDebug() << _interpreter->getLastErrorMessage();
        return(SYSTEM_FAIL);
    }
    /// initialization parser

    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::reInitialization()
{
    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::setToolFile(QString tool_file)
{
    QString tool_path;
    QString tool_name;
    tool_path.clear();
    tool_name.clear();
    for(int i = 0;i < tool_file.lastIndexOf("/");i++)
    {
        tool_path.append(tool_file.at(i));
    }
    for(int i = tool_file.lastIndexOf("/") + 1;i < tool_file.length();i++)
    {
        tool_name.append(tool_file.at(i));
    }
    _interpreter_settings.tool_path.clear();
    _interpreter_settings.tool_name.clear();
    _interpreter_settings.tool_path = tool_path;
    _interpreter_settings.tool_name = tool_name;
    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::setParamFile(QString param_file)
{
    QString param_path;
    QString param_name;
    param_path.clear();
    param_name.clear();
    for(int i = 0;i < param_file.lastIndexOf("/");i++)
    {
        param_path.append(param_file.at(i));
    }
    for(int i = param_file.lastIndexOf("/") + 1;i < param_file.length();i++)
    {
        param_name.append(param_file.at(i));
    }
    _interpreter_settings.param_path.clear();
    _interpreter_settings.param_name.clear();
    _interpreter_settings.param_path = param_path;
    _interpreter_settings.param_name = param_name;
    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::setProgramFile(QString program_file)
{
    QString parogram_in_path;
    QString parogram_in_name;
    parogram_in_path.clear();
    parogram_in_name.clear();
    for(int i = 0;i < program_file.lastIndexOf("/");i++)
    {
        parogram_in_path.append(program_file.at(i));
    }
    for(int i = program_file.lastIndexOf("/") + 1;i < program_file.length();i++)
    {
        parogram_in_name.append(program_file.at(i));
    }
    _interpreter_settings.parogram_in_path.clear();
    _interpreter_settings.parogram_in_name.clear();
    _interpreter_settings.parogram_in_path = parogram_in_path;
    _interpreter_settings.parogram_in_name = parogram_in_name;
    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::runParser()
{
    PARESER_STATE state;
    state = _parser->execut(_interpreter_settings.parogram_out_path,
                            _interpreter_settings.parogram_in_name);
    switch(state)
    {
        case PARESER_OK             : break;
        case PARESER_FILE_NOT_OPEN  : return(SYSTEM_FAIL);
        case PARESER_FILE_NOT_EXIST : return(SYSTEM_FAIL);
    }
    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
SYSTEM_STATE SystemClass::runInterpreter()
{
    INTERPRETER_STATE state;
    /// TEMPER BEGIN
    _interpreter->setTolerance(2.0F);
    /// TEMPER END
    state = _interpreter->execute();
    if(state != INTERPRETER_OK)
        if(_debug)
            qDebug() << _interpreter->getLastErrorMessage();
    switch(state)
    {
        case INTERPRETER_OK              : break;
        case INTERPRETER_INIT_FAIL       : return(SYSTEM_FAIL);
        case INTERPRETER_FILE_NOT_EXIST  : return(SYSTEM_FAIL);
        case INTERPRETER_FILE_NOT_OPEN   : return(SYSTEM_FAIL);
        case INTERPRETER_FILE_NOT_CREATE : return(SYSTEM_FAIL);
        case INTERPRETER_FILE_ERROR      : return(SYSTEM_FAIL);
    }
    return(SYSTEM_OK);
}
///-----------------------------------------------------------------------------
