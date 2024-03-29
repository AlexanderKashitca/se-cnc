///-----------------------------------------------------------------------------
#include "interpreter.h"
///-----------------------------------------------------------------------------
#include <QDebug>
///-----------------------------------------------------------------------------
using namespace INTERPRETER_SPACE;
///-----------------------------------------------------------------------------
static rs274ngcClass    rs274ngc;
static CannonInOutClass io;
///-----------------------------------------------------------------------------
InterpreterClass::InterpreterClass()
{
    _debug  = false;
    _tool_file.clear();
    _parameter_file.clear();
    _program_in_file.clear();
    _program_out_file.clear();
    _output_program_file = nullptr;
}
///-----------------------------------------------------------------------------
InterpreterClass::~InterpreterClass()
{
    _output_program_file->close();
    delete _output_program_file;
}
///-----------------------------------------------------------------------------
void InterpreterClass::setDebug(bool enable)
{
    _debug = enable;
    io.setDebug(enable);
}
///-----------------------------------------------------------------------------
bool InterpreterClass::getDebug()
{
    return(_debug);
}
///-----------------------------------------------------------------------------
INTERPRETER_STATE InterpreterClass::initialization(INTERPRETER_SETTINGS* settings)
{
    INTERPRETER_STATE status;

    status = setToolFile(settings->tool_path,settings->tool_name);
    if(status != INTERPRETER_OK)
        return(status);
    status = readToolFile();
    if(status != INTERPRETER_OK)
        return(status);
    status = setParameterFile(settings->param_path,settings->param_name);
    if(status != INTERPRETER_OK)
        return(status);
    status = setProgramInFile(settings->parogram_in_path,settings->parogram_in_name);
    if(status != INTERPRETER_OK)
        return(status);
    status = setProgramOutFile(settings->parogram_out_path,settings->parogram_out_name);
    return(status);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::SetTolerance
 * @param tolerance
 */
void InterpreterClass::setTolerance(double tolerance)
{
    rs274ngc.SetTolerance(tolerance);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::SetToolFile
 * @param file_path - path to tool file
 * @param file_name - tool file name
 * @return INTERPRETER_STATE
 */
INTERPRETER_STATE InterpreterClass::setToolFile(QString file_path,QString file_name)
{
    _tool_file.clear();
    _tool_file.append(file_path);
    _tool_file.append("/");
    _tool_file.append(file_name);
    if(!QFile::exists(_tool_file))
    {
        return(INTERPRETER_FILE_NOT_EXIST);
    }
    return(INTERPRETER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::SetProgramInFile
 * @param file_path - path to program input file
 * @param file_name - program file name
 * @return INTERPRETER_STATE
 */
INTERPRETER_STATE InterpreterClass::setProgramInFile(QString file_path,QString file_name)
{
    _program_in_file.clear();
    _program_in_file.append(file_path);
    _program_in_file.append("/");
    _program_in_file.append(file_name);

    if(!QFile::exists(_program_in_file))
    {
        return(INTERPRETER_FILE_NOT_EXIST);
    }
    return(INTERPRETER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::SetProgramOutFile
 * @param file_path - path to program output file
 * @param file_name - program file name
 * @return INTERPRETER_STATE
 */
INTERPRETER_STATE InterpreterClass::setProgramOutFile(QString file_path,QString file_name)
{
    QFile file_out;
    _program_out_file = file_name;
    _program_out_file.clear();
    _program_out_file.append(file_path);
    _program_out_file.append("/");
    _program_out_file.append(file_name);
    /// set file name
    file_out.setFileName(_program_out_file);
    if(!file_out.open(QIODevice::WriteOnly))
    {
        return(INTERPRETER_FILE_NOT_CREATE);
    }
    if(!QFile::exists(_program_out_file))
    {
        return(INTERPRETER_FILE_NOT_EXIST);
    }
    ///_output_progra_file = stdout;
    _output_program_file = new QFile;
    if(_output_program_file == nullptr)
    {
        return(INTERPRETER_FILE_ERROR);
    }
    _output_program_file->setFileName(_program_out_file);
    if(!_output_program_file->open(QIODevice::WriteOnly))
    {
        return(INTERPRETER_FILE_NOT_CREATE);
    }
    io.SetOutFile(_output_program_file);
    file_out.close();
    return(INTERPRETER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::SetParameterFile
 * @param file_path - path to parameter file
 * @param file_name - parameter file name
 * @return INTERPRETER_STATE
 */
INTERPRETER_STATE InterpreterClass::setParameterFile(QString file_path,QString file_name)
{
    _parameter_file.clear();
    _parameter_file.append(file_path);
    _parameter_file.append("/");
    _parameter_file.append(file_name);
    if(!QFile::exists(_parameter_file))
    {
        return(INTERPRETER_FILE_NOT_EXIST);
    }
    /// check size
    if(_parameter_file.size() > PARAMETER_FILE_NAME_SIZE)
    {
        return(INTERPRETER_FILE_ERROR);
    }
    /// set massive to zero
    for(int i = 0;i < PARAMETER_FILE_NAME_SIZE;i++)
    {
        io._parameter_file_name[i] = 0x00;
    }
    /// copy file name to driver part
    for(int i = 0;i < _parameter_file.size();i++)
    {
        io._parameter_file_name[i] = _parameter_file.at(i).toLatin1();
    }
    return(INTERPRETER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::ReadToolFile
 * @return INTERPRETER_STATE
 * @note   reding tool file and set tool table structure
 */
INTERPRETER_STATE InterpreterClass::readToolFile()
{
    QFile       file_in;
    QTextStream in(&file_in);
    QString     line;

    int     field;
    QString slot;
    QString id;
    QString length;
    QString diameter;
    QString holder;
    QString description;

    int    tool_slot;
    int    tool_id = 0;
    double tool_offset = 0.0;
    double tool_diameter = 0.0;

    for(tool_slot = 0;tool_slot <= io._tool_max;tool_slot++)
    {
        io._tools[tool_slot]._id       = -1;
        io._tools[tool_slot]._length   = 0.0;
        io._tools[tool_slot]._diameter = 0.0;
    }
    /// open tool file
    file_in.setFileName(_tool_file);
    if(!file_in.open(QIODevice::ReadOnly))
    {
        return(INTERPRETER_FILE_NOT_OPEN);
    }
    /// reading tool file and set tool parameter table
    if(_debug)
        qDebug() << "============ READ TOOL TABLE FILE ============ ";
    while(!in.atEnd())
    {
        line = in.readLine();
        field = 1;
        id.clear();
        slot.clear();
        length.clear();
        holder.clear();
        diameter.clear();
        description.clear();
        for(int i = 0;i < line.length();i++)
        {
            if(line.at(i) != '\t')
            {
                switch(field)
                {
                    case 1 :        slot.append(line.at(i)); break;
                    case 2 :          id.append(line.at(i)); break;
                    case 3 :      length.append(line.at(i)); break;
                    case 4 :    diameter.append(line.at(i)); break;
                    case 5 :      holder.append(line.at(i)); break;
                    case 6 : description.append(line.at(i)); break;
                }
            }
            else
                field++;
        }
        if(field != 6)
        {
            return(INTERPRETER_FILE_ERROR);
        }
        tool_slot     = slot.toInt();
        tool_id       = id.toInt();
        tool_offset   = length.toDouble();
        tool_diameter = diameter.toDouble();
        if((tool_slot < 0) || (tool_slot > io._tool_max))
        {
            return(INTERPRETER_FILE_ERROR);
        }
        io._tools[tool_slot]._id       = tool_id;
        io._tools[tool_slot]._length   = tool_offset;
        io._tools[tool_slot]._diameter = tool_diameter;
        if(_debug)
        {
            qDebug() << " TOOL TABLE LINE - "
                     << tool_id       << '\t'
                     << tool_offset   << '\t'
                     << tool_diameter << '\t'
                    ;
        }

    }
    if(_debug)
        qDebug() << "============================================== ";
    return(INTERPRETER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::Execute
 * @return
 */
INTERPRETER_STATE InterpreterClass::execute()
{
    int status;
    char buffer[80];
    int print_stack;
    INTERPRETER_STATE state;
    print_stack = OFF;
    if((status = rs274ngc.rs274ngc_init()) != RS274NGC_OK)
    {
        reportError(status, print_stack);
        return(INTERPRETER_INIT_FAIL);
    }
    status = rs274ngc.rs274ngc_open(_program_in_file.toStdString().c_str());
    if(status != RS274NGC_OK) /* do not need to close since not open */
    {
        reportError(status,print_stack);
        return(INTERPRETER_FILE_NOT_OPEN);
    }
    state = loadFromFile(print_stack);
    if(state != INTERPRETER_OK)
        return(state);
    rs274ngc.rs274ngc_file_name(buffer,5);  /* called to exercise the function */
    rs274ngc.rs274ngc_file_name(buffer,79); /* called to exercise the function */
    rs274ngc.rs274ngc_close();
    rs274ngc.rs274ngc_line_length();         /* called to exercise the function */
    rs274ngc.rs274ngc_sequence_number();     /* called to exercise the function */
    rs274ngc.rs274ngc_active_g_codes(_gees);  /* called to exercise the function */
    rs274ngc.rs274ngc_active_m_codes(_ems);   /* called to exercise the function */
    rs274ngc.rs274ngc_active_settings(_sets); /* called to exercise the function */
    rs274ngc.rs274ngc_exit(); /* saves parameters */
    return(INTERPRETER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::ReportError
 * @param error_code  - the code number of the error message
 * @param print_stack - print stack if ON, otherwise not
 */
void InterpreterClass::reportError(int error_code,int print_stack)
{
    if(_debug)
    {
        char buffer[RS274NGC_TEXT_SIZE];
        int k;
        rs274ngc.rs274ngc_error_text(error_code,buffer,5); /* for coverage of code */
        rs274ngc.rs274ngc_error_text(error_code,buffer,RS274NGC_TEXT_SIZE);
        _errMsg.clear();
        _errMsg.append(buffer);
        fprintf(stderr,"%s\n",((buffer[0] == 0) ? "Unknown error, bad error code" : buffer));
        rs274ngc.rs274ngc_line_text(buffer, RS274NGC_TEXT_SIZE);
        fprintf(stderr,"%s\n",buffer);
        if(print_stack == ON)
        {
            for(k = 0;;k++)
            {
                rs274ngc.rs274ngc_stack_name(k,buffer,RS274NGC_TEXT_SIZE);
                if(buffer[0] != 0)
                    fprintf(stderr,"%s\n",buffer);
                else
                    break;
            }
        }
    }

}
///-----------------------------------------------------------------------------
/**
 * @brief interpret_from_file
 * @param print_stack  - option which is ON or OFF
 * @return
 */
INTERPRETER_STATE InterpreterClass::loadFromFile(int print_stack)
{
    int status;
    while(1)
    {
        status = rs274ngc.rs274ngc_read(nullptr);
        if(status == RS274NGC_ENDFILE)
            break;
        if((status != RS274NGC_OK) && (status != RS274NGC_EXECUTE_FINISH))
        {
            reportError(status,print_stack);
            break;
        }
        status = rs274ngc.rs274ngc_execute();
        if((status != RS274NGC_OK) && (status != RS274NGC_EXIT) && (status != RS274NGC_EXECUTE_FINISH))
        {
            reportError(status,print_stack);
            break;
        }
        else
            if(status == RS274NGC_EXIT)
                break;
    }
    switch(status)
    {
        case RS274NGC_OK             : return(INTERPRETER_OK);
        case RS274NGC_EXIT           : return(INTERPRETER_FILE_ERROR);
        case RS274NGC_EXECUTE_FINISH : return(INTERPRETER_OK);
        case RS274NGC_ENDFILE        : return(INTERPRETER_OK);
        default                      : return(INTERPRETER_FILE_ERROR);
    }
}
///-----------------------------------------------------------------------------
QString InterpreterClass::getLastErrorMessage()
{
    return(_errMsg);
}
///-----------------------------------------------------------------------------
