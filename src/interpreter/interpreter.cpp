///-----------------------------------------------------------------------------
#include "interpreter.h"
///-----------------------------------------------------------------------------
#include <QDebug>
///-----------------------------------------------------------------------------
static rs274ngcClass    rs274ngc;
static CannonInOutClass io;
///-----------------------------------------------------------------------------
InterpreterClass::InterpreterClass()
{
    _tool_file.clear();
    _parameter_file.clear();
    _program_in_file.clear();
    _program_out_file.clear();
    _output_progra_file = nullptr;
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::SetToolFile
 * @param file_path - path to tool file
 * @param file_name - tool file name
 * @return INTERPRETER_STATE
 */
INTERPRETER_STATE InterpreterClass::SetToolFile(QString file_path,QString file_name)
{
    _tool_file = file_path + "/" + file_name;
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
INTERPRETER_STATE InterpreterClass::SetProgramInFile(QString file_path,QString file_name)
{
    _program_in_file.clear();
    _program_in_file.append(file_path);
    _program_in_file.append("/");
    _program_in_file.append(file_name);

    qDebug() << "_program_in_file size - " << _program_in_file.size() << endl;

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
#include <stdio.h>
#include <iostream>
INTERPRETER_STATE InterpreterClass::SetProgramOutFile(QString file_path,QString file_name)
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
    _output_progra_file = new FILE;
    if(_output_progra_file == nullptr)
    {
        return(INTERPRETER_FILE_ERROR);
    }
    _output_progra_file = fopen(_program_out_file.toStdString().c_str(),"w");

    io.SetOutFile(_output_progra_file);
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
INTERPRETER_STATE InterpreterClass::SetParameterFile(QString file_path,QString file_name)
{
    _parameter_file = file_name;
    _parameter_file = file_path + "/" + file_name;
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
INTERPRETER_STATE InterpreterClass::ReadToolFile()
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
        io._tools[tool_slot].id       = -1;
        io._tools[tool_slot].length   = 0.0;
        io._tools[tool_slot].diameter = 0.0;
    }
    /// open tool file
    file_in.setFileName(_tool_file);
    if(!file_in.open(QIODevice::ReadOnly))
    {
        return(INTERPRETER_FILE_NOT_OPEN);
    }
    /// reading tool file and set tool parameter table
    qDebug() << "============ READ TOOL TABLE FILE ============ " << endl;
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
        io._tools[tool_slot].id       = tool_id;
        io._tools[tool_slot].length   = tool_offset;
        io._tools[tool_slot].diameter = tool_diameter;
        qDebug() << " TOOL TABLE LINE - "
                 << tool_id       << '\t'
                 << tool_offset   << '\t'
                 << tool_diameter << '\t'
                 << endl;
    }
    return(INTERPRETER_OK);
}
///-----------------------------------------------------------------------------
/**
 * @brief InterpreterClass::Execute
 * @return
 */
void report_error(int error_code,int print_stack);
int interpret_from_file(int do_next,int block_delete,int print_stack);
INTERPRETER_STATE InterpreterClass::Execute()
{
    int status;
    int choice;
    int do_next; /* 0=continue, 1=mdi, 2=stop */
    int block_delete;
    char buffer[80];


    int print_stack;


    do_next = 2;  /* 2=stop */
    block_delete = OFF;
    print_stack = OFF;

    if((status = rs274ngc.rs274ngc_init()) != RS274NGC_OK)
    {
        report_error(status, print_stack);
        return(INTERPRETER_INIT);
    }

    status = rs274ngc.rs274ngc_open(_program_in_file.toStdString().c_str());
    if(status != RS274NGC_OK) /* do not need to close since not open */
    {
        report_error(status, print_stack);
        return(INTERPRETER_FILE_NOT_OPEN);
    }
    status = interpret_from_file(do_next, block_delete, print_stack);
    rs274ngc.rs274ngc_file_name(buffer, 5);  /* called to exercise the function */
    rs274ngc.rs274ngc_file_name(buffer, 79); /* called to exercise the function */
    rs274ngc.rs274ngc_close();

}
///-----------------------------------------------------------------------------
void report_error( /* ARGUMENTS                            */
 int error_code,   /* the code number of the error message */
 int print_stack)  /* print stack if ON, otherwise not     */
{
  char buffer[RS274NGC_TEXT_SIZE];
  int k;

  rs274ngc.rs274ngc_error_text(error_code, buffer, 5); /* for coverage of code */
  rs274ngc.rs274ngc_error_text(error_code, buffer, RS274NGC_TEXT_SIZE);
  fprintf(stderr, "%s\n",
      ((buffer[0] == 0) ? "Unknown error, bad error code" : buffer));
  rs274ngc.rs274ngc_line_text(buffer, RS274NGC_TEXT_SIZE);
  fprintf(stderr, "%s\n", buffer);
  if (print_stack == ON)
    {
      for (k = 0; ; k++)
    {
      rs274ngc.rs274ngc_stack_name(k, buffer, RS274NGC_TEXT_SIZE);
      if (buffer[0] != 0)
        fprintf(stderr, "%s\n", buffer);
      else
        break;
    }
    }
}
///-----------------------------------------------------------------------------
int interpret_from_file( /* ARGUMENTS                  */
 int do_next,            /* what to do if error        */
 int block_delete,       /* switch which is ON or OFF  */
 int print_stack)        /* option which is ON or OFF  */
{
  int status;
  char line[RS274NGC_TEXT_SIZE];

    for(;;)
    {
        status = rs274ngc.rs274ngc_read(nullptr);
        if((status == RS274NGC_EXECUTE_FINISH) && (block_delete == ON))
            continue;
        else
            if(status == RS274NGC_ENDFILE)
                break;
        if((status != RS274NGC_OK) && (status != RS274NGC_EXECUTE_FINISH))
        {
            report_error(status, print_stack);
            //if((status == NCE_FILE_ENDED_WITH_NO_PERCENT_SIGN) || (do_next == 2))
            //{
            //    status = 1;
            //    break;
            //}
            //else
            //    if(do_next == 1) /* 1 means MDI */
            //    {
            //        fprintf(stderr, "starting MDI\n");
            //        interpret_from_keyboard(block_delete, print_stack);
            //        fprintf(stderr, "continue program? y/n =>");
            //        gets(line);
            //        if(line[0] != 'y')
            //        {
            //            status = 1;
            //            break;
            //        }
            //        else
            //        continue;
            //    }
            //    else
            //        continue;
        }
        status = rs274ngc.rs274ngc_execute();
        if((status != RS274NGC_OK) && (status != RS274NGC_EXIT) && (status != RS274NGC_EXECUTE_FINISH))
        {
            report_error(status, print_stack);
            //status = 1;
            //if(do_next == 1) /* 1 means MDI */
            //{
            //    fprintf(stderr, "starting MDI\n");
            //    interpret_from_keyboard(block_delete, print_stack);
            //    fprintf(stderr, "continue program? y/n =>");
            //    gets(line);
            //    if(line[0] != 'y')
            //        break;
            //}
            //else
            //    if(do_next == 2) /* 2 means stop */
            //        break;
        }
        else
            if(status == RS274NGC_EXIT)
                break;
    }
    return ((status == 1) ? 1 : 0);
}



#if 0

int status;
int choice;
int do_next; /* 0=continue, 1=mdi, 2=stop */
int block_delete;
char buffer[80];

char default_name[] = "rs274ngc.var";
int print_stack;

if (argc > 3)
  {
    fprintf(stderr, "Usage \"%s\"\n", argv[0]);
    fprintf(stderr, "   or \"%s <input file>\"\n", argv[0]);
    fprintf(stderr, "   or \"%s <input file> <output file>\"\n", argv[0]);
    exit(1);
  }

do_next = 2;  /* 2=stop */
block_delete = OFF;
print_stack = OFF;
tool_flag = 0;
strcpy(io._parameter_file_name, default_name);
io.SetOutFile(stdout); /* may be reset below */



for(; ;)
  {
    fprintf(stderr, "enter a number:\n");
    fprintf(stderr, "1 = start interpreting\n");
    fprintf(stderr, "2 = choose parameter file ...\n");
    fprintf(stderr, "3 = read tool file ...\n");
    fprintf(stderr, "4 = turn block delete switch %s\n",
        ((block_delete == OFF) ? "ON" : "OFF"));
    fprintf(stderr, "5 = adjust error handling...\n");
    fprintf(stderr, "enter choice => ");
    gets(buffer);
    if (sscanf(buffer, "%d", &choice) != 1)
  continue;
    if (choice == 1)
  break;
    else if (choice == 2)
  {
    if (designate_parameter_file(io._parameter_file_name) != 0)
      exit(1);
  }
    else if (choice == 3)
  {
    if (read_tool_file("") != 0)
      exit(1);
    tool_flag = 1;
  }
    else if (choice == 4)
  block_delete = ((block_delete == OFF) ? ON : OFF);
    else if (choice == 5)
  adjust_error_handling(argc, &print_stack, &do_next);
  }

fprintf(stderr, "executing\n");


/*
if (argc == 3)
  {
    _outfile = fopen(argv[2], "w");
    if (_outfile == nullptr)
  {
    fprintf(stderr, "could not open output file %s\n", argv[2]);
    exit(1);
  }
  }
*/
if ((status = rs274ngc.rs274ngc_init()) != RS274NGC_OK)
  {
    report_error(status, print_stack);
    exit(1);
  }

if (argc == 1)
  status = interpret_from_keyboard(block_delete, print_stack);
else /* if (argc IS 2 or argc IS 3) */
  {
    status = rs274ngc.rs274ngc_open(argv[1]);
    if (status != RS274NGC_OK) /* do not need to close since not open */
  {
    report_error(status, print_stack);
    exit(1);
  }
    status = interpret_from_file(do_next, block_delete, print_stack);
    rs274ngc.rs274ngc_file_name(buffer, 5);  /* called to exercise the function */
    rs274ngc.rs274ngc_file_name(buffer, 79); /* called to exercise the function */
    rs274ngc.rs274ngc_close();
  }
rs274ngc.rs274ngc_line_length();         /* called to exercise the function */
rs274ngc.rs274ngc_sequence_number();     /* called to exercise the function */
rs274ngc.rs274ngc_active_g_codes(gees);  /* called to exercise the function */
rs274ngc.rs274ngc_active_m_codes(ems);   /* called to exercise the function */
rs274ngc.rs274ngc_active_settings(sets); /* called to exercise the function */
rs274ngc.rs274ngc_exit(); /* saves parameters */

  qDebug() << "exit status - " << status << endl; /// exit(status);




  ///-----------------------------------------------------------------------------

  ///-----------------------------------------------------------------------------
  /*

  This file contains the source code for an emulation of using the six-axis
  rs274 interpreter from the EMC system.

  */

  /*********************************************************************/

  /* report_error

  Returned Value: none

  Side effects: an error message is printed on stderr

  Called by:
    interpret_from_file
    interpret_from_keyboard
    main
  This

  1. calls rs274ngc_error_text to get the text of the error message whose
  code is error_code and prints the message,

  2. calls rs274ngc_line_text to get the text of the line on which the
  error occurred and prints the text, and

  3. if print_stack is on, repeatedly calls rs274ngc_stack_name to get
  the names of the functions on the function call stack and prints the
  names. The first function named is the one that sent the error
  message.


  */



  /***********************************************************************/

  /* interpret_from_keyboard

  Returned Value: int (0)

  Side effects:
    Lines of NC code entered by the user are interpreted.

  Called by:
    interpret_from_file
    main

  This prompts the user to enter a line of rs274 code. When the user
  hits <enter> at the end of the line, the line is executed.
  Then the user is prompted to enter another line.

  Any canonical commands resulting from executing the line are printed
  on the monitor (stdout).  If there is an error in reading or executing
  the line, an error message is printed on the monitor (stderr).

  To exit, the user must enter "quit" (followed by a carriage return).

  */

  int interpret_from_keyboard(  /* ARGUMENTS                 */
   int block_delete,            /* switch which is ON or OFF */
   int print_stack)             /* option which is ON or OFF */
  {
    char line[RS274NGC_TEXT_SIZE];
    int status;

      for(; ;)
      {
          printf("READ => ");
          gets(line);
          if(strcmp (line, "quit") == 0)
              return 0;
          status = rs274ngc.rs274ngc_read(line);
          if((status == RS274NGC_EXECUTE_FINISH) && (block_delete == ON));
          else if (status == RS274NGC_ENDFILE);
          else if ((status != RS274NGC_EXECUTE_FINISH) &&
              (status != RS274NGC_OK))
          report_error(status, print_stack);
          else
          {
              status = rs274ngc.rs274ngc_execute();
              if ((status == RS274NGC_EXIT) || (status == RS274NGC_EXECUTE_FINISH));
              else if (status != RS274NGC_OK)
                  report_error(status, print_stack);
          }
      }
  }

  /*********************************************************************/

  /* interpret_from_file

  Returned Value: int (0 or 1)
     If any of the following errors occur, this returns 1.
     Otherwise, it returns 0.
     1. rs274ngc_read returns something other than RS274NGC_OK or
        RS274NGC_EXECUTE_FINISH, no_stop is off, and the user elects
        not to continue.
     2. rs274ngc_execute returns something other than RS274NGC_OK,
        EXIT, or RS274NGC_EXECUTE_FINISH, no_stop is off, and the user
        elects not to continue.

  Side Effects:
     An open NC-program file is interpreted.

  Called By:
     main

  This emulates the way the EMC system uses the interpreter.

  If the do_next argument is 1, this goes into MDI mode if an error is
  found. In that mode, the user may (1) enter code or (2) enter "quit" to
  get out of MDI. Once out of MDI, this asks the user whether to continue
  interpreting the file.

  If the do_next argument is 0, an error does not stop interpretation.

  If the do_next argument is 2, an error stops interpretation.

  */


  /************************************************************************/

  /* read_tool_file

  Returned Value: int
    If any of the following errors occur, this returns 1.
    Otherwise, it returns 0.
    1. The file named by the user cannot be opened.
    2. No blank line is found.
    3. A line of data cannot be read.
    4. A tool slot number is less than 1 or >= _tool_max

  Side Effects:
    Values in the tool table of the machine setup are changed,
    as specified in the file.

  Called By: main

  Tool File Format
  -----------------
  Everything above the first blank line is read and ignored, so any sort
  of header material may be used.

  Everything after the first blank line should be data. Each line of
  data should have four or more items separated by white space. The four
  required items are slot, tool id, tool length offset, and tool diameter.
  Other items might be the holder id and tool description, but these are
  optional and will not be read. Here is a sample line:

  20  1419  4.299  1.0   1 inch carbide end mill

  The tool_table is indexed by slot number.

  */

  int read_tool_file(  /* ARGUMENTS         */
   char * file_name)   /* name of tool file */
  {
    FILE * tool_file_port;
    char buffer[1000];
    int slot;
    int tool_id;
    double offset;
    double diameter;

    if (file_name[0] == 0) /* ask for name if given name is empty string */
      {
        fprintf(stderr, "name of tool file => ");
        gets(buffer);
        tool_file_port = fopen(buffer, "r");
      }
    else
        tool_file_port = fopen(file_name, "r");
    if (tool_file_port == nullptr)
      {
        fprintf(stderr, "Cannot open %s\n",
            ((file_name[0] == 0) ? buffer : file_name));
        return 1;
      }
    for(;;)    /* read and discard header, checking for blank line */
      {
        if (fgets(buffer, 1000, tool_file_port) == nullptr)
      {
        fprintf(stderr, "Bad tool file format\n");
        return 1;
      }
        else if (buffer[0] == '\n')
      break;
      }

    for (slot = 0; slot <= io._tool_max; slot++) /* initialize */
      {
        io._tools[slot].id = -1;
        io._tools[slot].length = 0;
        io._tools[slot].diameter = 0;
      }
    for (; (fgets(buffer, 1000, tool_file_port) != nullptr); )
      {
        if (sscanf(buffer, "%d %d %lf %lf", &slot,
           &tool_id, &offset, &diameter) < 4)
      {
        fprintf(stderr, "Bad input line \"%s\" in tool file\n", buffer);
        return 1;
      }
        if ((slot < 0) || (slot > io._tool_max)) /* zero and max both OK */
      {
        fprintf(stderr, "Out of range tool slot number %d\n", slot);
        return 1;
      }
        io._tools[slot].id = tool_id;
        io._tools[slot].length = offset;
        io._tools[slot].diameter = diameter;
      }
    fclose(tool_file_port);
    return 0;
  }
  /************************************************************************/

  /* designate_parameter_file

  Returned Value: int
    If any of the following errors occur, this returns 1.
    Otherwise, it returns 0.
    1. The file named by the user cannot be opened.

  Side Effects:
    The name of a parameter file given by the user is put in the
    file_name string.

  Called By: main

  */

  int designate_parameter_file(char * file_name)
  {
    FILE * test_port;

    fprintf(stderr, "name of parameter file => ");
    gets(file_name);
    test_port = fopen(file_name, "r");
    if (test_port == nullptr)
      {
        fprintf(stderr, "Cannot open %s\n", file_name);
        return 1;
      }
    fclose(test_port);
    return 0;
  }
  /************************************************************************/

  /* adjust_error_handling

  Returned Value: int (0)

  Side Effects:
    The values of print_stack and do_next are set.

  Called By: main

  This function allows the user to set one or two aspects of error handling.

  By default the driver does not print the function stack in case of error.
  This function always allows the user to turn stack printing on if it is off
  or to turn stack printing off if it is on.

  When interpreting from the keyboard, the driver always goes ahead if there
  is an error.

  When interpreting from a file, the default behavior is to stop in case of
  an error. If the user is interpreting from a file (indicated by args being
  2 or 3), this lets the user change what it does on an error.

  If the user has not asked for output to a file (indicated by args being 2),
  the user can choose any of three behaviors in case of an error (1) continue,
  (2) stop, (3) go into MDI mode. This function allows the user to cycle among
  the three.

  If the user has asked for output to a file (indicated by args being 3),
  the user can choose any of two behaviors in case of an error (1) continue,
  (2) stop. This function allows the user to toggle between the two.

  */

  int adjust_error_handling(
   int args,
   int * print_stack,
   int * do_next)
  {
    char buffer[80];
    int choice;

    for(;;)
      {
        fprintf(stderr, "enter a number:\n");
        fprintf(stderr, "1 = done with error handling\n");
        fprintf(stderr, "2 = %sprint stack on error\n",
            ((*print_stack == ON) ? "do not " : ""));
        if (args == 3)
      {
        if (*do_next == 0) /* 0 means continue */
          fprintf(stderr,
              "3 = stop on error (do not continue)\n");
        else /* if do_next IS 2 -- 2 means stopping on error */
          fprintf(stderr,
              "3 = continue on error (do not stop)\n");
      }
        else if (args == 2)
      {
        if (*do_next == 0) /* 0 means continue */
          fprintf(stderr,
              "3 = mdi on error (do not continue or stop)\n");
        else if (*do_next == 1) /* 1 means MDI */
          fprintf(stderr,
              "3 = stop on error (do not mdi or continue)\n");
        else /* if do_next IS 2 -- 2 means stopping on error */
          fprintf(stderr,
              "3 = continue on error (do not stop or mdi)\n");
      }
        fprintf(stderr, "enter choice => ");
        gets(buffer);
        if (sscanf(buffer, "%d", &choice) != 1)
      continue;
        if (choice == 1)
      break;
        else if (choice == 2)
      *print_stack = ((*print_stack == OFF) ? ON : OFF);
        else if ((choice == 3) && (args == 3))
      *do_next = ((*do_next == 0) ? 2 : 0);
        else if ((choice == 3) && (args == 2))
      *do_next = ((*do_next == 2) ? 0 : (*do_next + 1));
      }
    return 0;
  }



#endif
