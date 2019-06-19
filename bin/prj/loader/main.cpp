#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}

#if 0
/*
/// Note: ALL User Thread Numbers start with 1
int LoadCoff(int Thread,const char *Name,int PackToFlash = 0); //PackToFlash 0=normal,1=NewVersion,2=bootload
int CompileAndLoadCoff(const char *Name,int Thread);
int CompileAndLoadCoff(const char *Name,int Thread,char *Err,int MaxErrLen);
int Compile(const char *Name,const char *OutFile,const int board_type,int Thread,char *Err,int MaxErrLen);
int CompileTI(const char *Name,const char *OutFile,const int BoardType,int Thread,char *Err,int MaxErrLen);
int LinkTI(const char *Linker,const char *Name,const char *OutFile,const int BoardType,int Thread,char * Err,int MaxErrLen,int MaxSize);
int ValidateC(const char *Name,char *Err,int MaxErrLen);
int CheckCoffSize(const char *Name,int *size_text,int *size_bss,int *size_data,int *size_total);  // return size of sections and total (including padding)
unsigned int GetLoadAddress(int thread,int BoardType);
void ConvertToOut(int thread,const char *InFile,char *OutFile,int MaxLength);
void RemoveComments(QString &s);
///        int CKMotionDLL::ExtractCoffVersionString(const char *InFile,char *Version);
*/

// Note: ALL User Thread Numbers start with 1

int SEMotionClass::LoadCoff(int Thread, const char *Name, int PackToFlash)
{
#if 0
    QString s;
    unsigned int EntryPoint;

    if (Thread==0) return 1;

    if (PackToFlash==0 && CheckKMotionVersion()) return 1;

    if (PackToFlash==0)
    {
        s = QString("Kill %d").arg(Thread);
        ///s.Format("Kill %d", Thread);  // make sure the Thread isn't running
        if (WriteLine(s.toStdString().c_str())) return 1;
    }

    int result =  ::LoadCoff(this, Name, &EntryPoint, PackToFlash);

    if (result) return result;

    if (Thread >= 0 && PackToFlash==0)
    {
        // Set the entry point for the thread
        s = QString("EntryPoint%d %X")
                .arg(Thread)
                .arg(EntryPoint);
        ///s.Format("EntryPoint%d %X",Thread,EntryPoint);
        result = WriteLine(s.toStdString().c_str());
        if (result) return result;
    }
#endif
    return 0;
}
#if 0
///-----------------------------------------------------------------------------
int SEMotionClass::CompileAndLoadCoff(const char *Name, int Thread)
{
    return CompileAndLoadCoff(Name, Thread, NULL, 0);
}
///-----------------------------------------------------------------------------
int SEMotionClass::CompileAndLoadCoff(const char *Name, int Thread, char *Err, int MaxErrLen)
{
    int result,BoardType;
    CString OutFile;
    CString BindTo;


    if (Thread<=0 || Thread>7)
    {
        CString s;
        s.Format("Invalid Thread Number %d Valid Range (1-7)",Thread);
        strcpy(Err,s);
        return 1;
    }

    ConvertToOut(Thread,Name,OutFile.GetBuffer(MAX_PATH),MAX_PATH);
    OutFile.ReleaseBuffer();

    if (CheckKMotionVersion(&BoardType)) return 1;

    // Compile the C File

    result = Compile(Name,OutFile,BoardType,Thread,Err,MaxErrLen);
    if (result) return result;

    // Download the .out File

    result = LoadCoff(Thread, OutFile);
    if (result) return result;
#endif
    return 0;
}
///-----------------------------------------------------------------------------
int SEMotionClass::Compile(const char *Name, const char *OutFile, const int BoardType, int Thread, char *Err, int MaxErrLen)
{
#if 0
    SECURITY_ATTRIBUTES sa          = {0};
    STARTUPINFO         si          = {0};
    PROCESS_INFORMATION pi          = {0};
    HANDLE              hPipeOutputRead  = NULL;
    HANDLE              hPipeOutputWrite = NULL;
    HANDLE              hPipeInputRead   = NULL;
    HANDLE              hPipeInputWrite  = NULL;
    BOOL                bTest = 0;
    DWORD               dwNumberOfBytesRead = 0;
    CHAR                szMsg[100];

    CString Errors;

    if (Thread==0) return 1;

    // Check for #Pragma TI_COMPILER

    FILE *f=fopen(Name, "rt");

    if (f)
    {
        CString s;
        fgets(s.GetBufferSetLength(200), 200, f);
        s.ReleaseBuffer();
        fclose(f);

        RemoveComments(s);
        s.TrimLeft();
        int i = s.Find("#pragma");
        if (i >= 0)
        {
            int k = s.Find("TI_COMPILER",i);
            if (k > i)
            {
                return CompileTI(Name, OutFile, BoardType, Thread, Err, MaxErrLen);
            }
        }
    }


    // Try and locate the TCC67 Compiler
    CString Compiler = MainPathDLL + COMPILER;
    CString OFile=OutFile;

    f=fopen(Compiler,"r");  // try where the KMotionDLL was first

    if (f==NULL)
    {
        Compiler = MainPath + "\\Release" + COMPILER;

        f=fopen(Compiler,"r");  // try in the released directory next
        if (f==NULL)
        {
            Compiler = MainPath + "\\Debug" + COMPILER;
            f=fopen(Compiler,"r");  // try in the debug directory next
            if (f==NULL)
            {
                DoErrMsg("Error Locating TCC67.exe Compiler");
                return 1;
            }
        }
    }
    fclose(f);


    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Create pipe for standard output redirection.
    CreatePipe(&hPipeOutputRead,  // read handle
        &hPipeOutputWrite, // write handle
        &sa,      // security attributes
        0      // number of bytes reserved for pipe - 0 default
        );

    // Create pipe for standard input redirection.
    CreatePipe(&hPipeInputRead,  // read handle
        &hPipeInputWrite, // write handle
        &sa,      // security attributes
        0      // number of bytes reserved for pipe - 0 default
        );

    // Make child process use hPipeOutputWrite as standard out,
    // and make sure it does not show on screen.
    si.cb = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput   = hPipeInputRead;
    si.hStdOutput  = hPipeOutputWrite;
    si.hStdError   = hPipeOutputWrite;

    CString cmd;  // build command line
    CString BindTo,IncSrcPath1,IncSrcPath2;

    if (BoardType == BOARD_TYPE_KMOTION)
        IncSrcPath1="-I \"" + MainPathRoot + "\\DSP_KMotion\" ";
    else
        IncSrcPath1="-I \"" + MainPathRoot + "\\DSP_KFLOP\" ";

    IncSrcPath2="-I \"" + ExtractPath(Name) + "\"";

    if (BoardType == BOARD_TYPE_KMOTION)
        BindTo = MainPathRoot + "\\DSP_KMotion\\DSPKMotion.out";
    else
        BindTo = MainPathRoot + "\\DSP_KFLOP\\DSPKFLOP.out";


    cmd.Format(" -text %08X -g -nostdinc " + IncSrcPath1 + IncSrcPath2 + " -o ",GetLoadAddress(Thread,BoardType));
    cmd = Compiler + cmd;
    cmd += "\"" + OFile + "\" \"" + Name + "\" \"" + BindTo +"\"";

    CreateProcess (
        NULL,
        cmd.GetBuffer(0),
        NULL, NULL,
        TRUE, 0,
        NULL, NULL,
        &si, &pi);


    // Now that handles have been inherited, close it to be safe.
    // You don't want to read or write to them accidentally.
    CloseHandle(hPipeOutputWrite);
    CloseHandle(hPipeInputRead);


    // Wait for CONSPAWN to finish.
    WaitForSingleObject (pi.hProcess, INFINITE);

    DWORD exitcode;
    int result = GetExitCodeProcess(pi.hProcess,&exitcode);

    // Now test to capture DOS application output by reading
    // hPipeOutputRead.  Could also write to DOS application
    // standard input by writing to hPipeInputWrite.

    if (exitcode==0)
    {
        Errors="";
    }
    else
    {
        char *s = Errors.GetBuffer(10001);

        bTest=ReadFile(
            hPipeOutputRead,      // handle of the read end of our pipe
            s,					  // address of buffer that receives data
            10000,                // number of bytes to read
            &dwNumberOfBytesRead, // address of number of bytes read
            NULL                  // non-overlapped.
            );


        if (!bTest)
        {
            wsprintf(szMsg, "Error #%d reading compiler output.",GetLastError());
            DoErrMsg(szMsg);
            return 1;
        }

        // do something with data.
        s[dwNumberOfBytesRead] = 0;  // null terminate
        Errors.ReleaseBuffer();
    }



    // Close all remaining handles
    CloseHandle (pi.hProcess);
    CloseHandle (pi.hThread);
    CloseHandle (hPipeOutputRead);
    CloseHandle (hPipeInputWrite);

    /*----------Console application (CONSPAWN.EXE) code------*/

    if (Err)
        strncpy(Err,Errors,MaxErrLen);

    return exitcode;
#endif
}
///-----------------------------------------------------------------------------
void SEMotionClass::RemoveComments(QString &s)
{
#if 0
    int i = s.Find("//");
    if (i >= 0)
        s = s.Left(i);

    i = s.Find("/*");
    if (i >= 0)
    {
        s = s.Left(s.GetLength() - i);

        int k = s.Find("*/", i);

        if (k > i)
        {   // both found remove section
            s.Delete(i, k - i + 2);
        }
        else // one found - remove remainder
        {
            s = s.Left(i);
        }
    }
#endif
}
///-----------------------------------------------------------------------------
int SEMotionClass::CompileTI(const char * Name, const char * OutFile, const int BoardType, int Thread, char * Err, int MaxErrLen)
{
#if 0
    SECURITY_ATTRIBUTES sa = { 0 };
    STARTUPINFO         si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    HANDLE              hPipeOutputRead = NULL;
    HANDLE              hPipeOutputWrite = NULL;
    HANDLE              hPipeInputRead = NULL;
    HANDLE              hPipeInputWrite = NULL;
    BOOL                bTest = 0;
    DWORD               dwNumberOfBytesRead = 0;
    CHAR                szMsg[100];

    CString Errors;

    int Opt = 0; // default no optimization
    int MaxSize = 0x10000;
    if (Thread == 7) MaxSize *= 5;

    FILE *f = fopen(Name, "rt");

    if (f)
    {
        CString s;
        fgets(s.GetBufferSetLength(200), 200, f);
        s.ReleaseBuffer();
        fclose(f);

        RemoveComments(s);
        s.TrimLeft();
        int i = s.Find("#pragma");
        if (i >= 0)
        {
            int k = s.Find("TI_COMPILER", i);
            if (k > i)
            {
                CString sOpt, sMaxSize;
                int r1 = 1, r2 = 1, i1, i2, i3, i0;
                i0 = s.Find("(", k);  // look for (Opt) or (Opt,MaxSize)
                if (i0 > k)
                {
                    i1 = s.Find(",", i0);  // look for (Opt) or (Opt,MaxSize)
                    if (i1 > i0)
                    {
                        i2 = s.Find(")", i1);  //  (Opt,MaxSize)
                        if (i2 > i1)
                        {
                            sOpt = s.Mid(i0 + 1, i1 - i0 - 1);
                            r1 = sscanf(sOpt, "%d", &Opt);
                            sMaxSize = s.Mid(i1 + 1, i2 - i1 - 1);

                            i3 = sMaxSize.Find("0x");
                            if (i3 >= 0)
                            {	// hex value
                                sMaxSize = sMaxSize.Right(sMaxSize.GetLength() - i3 - 2);
                                r2 = sscanf(sMaxSize, "%x", &MaxSize);
                            }
                            else
                            {	// decimal value
                                sMaxSize = s.Mid(i1 + 1, i2 - i1 - 1);
                                r2 = sscanf(sMaxSize, "%d", &MaxSize);
                            }
                        }
                    }
                    else
                    {
                        r1 = 0;
                        i1 = s.Find(")", i0);  // (Opt)
                        if (i1 > i0)
                        {
                            sOpt = s.Mid(i0 + 1, i1 - i0 - 1);
                            r1 = sscanf(sOpt, "%d", &Opt);
                        }
                    }
                }


                if (r1 != 1 || r2 != 1 || Opt < 0 || Opt > 3 || MaxSize <= 0 || MaxSize > 11 * 0x10000)
                {
                    // invalid format pragma TI_COMPILER

                    Errors = "Error Line :1: invalid TI_COMPILER pragma \n" + s +
                        "\n Expected format:"
                        "\n#pragma TI_COMPILER"
                        "\n#pragma TI_COMPILER(opt level 0-3)"
                        "\n#pragma TI_COMPILER(opt level 0-3, Max Thread Space - hex or decimal)";

                    strncpy(Err, Errors, MaxErrLen);
                    return 1;
                }
            }
        }
    }


    // Try and locate the Compiler
    CString Compiler = MainPathRoot + COMPILERTI;
    CString OFile = OutFile;

    f = fopen(Compiler, "r");  // try where the KMotionDLL was first

    if (f == NULL)
    {
        DoErrMsg("Error Locating cl6x.exe Compiler");
        return 1;
    }

    fclose(f);

    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Create pipe for standard output redirection.
    CreatePipe(&hPipeOutputRead,  // read handle
        &hPipeOutputWrite, // write handle
        &sa,      // security attributes
        10000000      // number of bytes reserved for pipe - 0 default
    );

    // Create pipe for standard input redirection.
    CreatePipe(&hPipeInputRead,  // read handle
        &hPipeInputWrite, // write handle
        &sa,      // security attributes
        10000000      // number of bytes reserved for pipe - 0 default
    );

    // Make child process use hPipeOutputWrite as standard out,
    // and make sure it does not show on screen.
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = hPipeInputRead;
    si.hStdOutput = hPipeOutputWrite;
    si.hStdError = hPipeOutputWrite;

    CString cmd;  // build command line
    CString IncSrcPath1, IncSrcPath2, opt;

    if (BoardType == BOARD_TYPE_KMOTION)
        IncSrcPath1 = "-i \"" + MainPathRoot + "\\DSP_KMotion\" ";
    else
        IncSrcPath1 = "-i \"" + MainPathRoot + "\\DSP_KFLOP\" ";

    IncSrcPath2 = "-i \"" + ExtractPath(Name) + "\"";

    cmd.Format(" -k -q -as --diag_suppress=163 " + IncSrcPath1 + IncSrcPath2 + " -mu -ml3 -mv6710 -o%d", Opt);
    cmd = Compiler + cmd + " \"" + Name + "\" --obj_directory=\"" + ExtractPath(Name) + "\" --asm_directory=\"" + ExtractPath(Name) + "\"";

    CreateProcess(
        NULL,
        cmd.GetBuffer(0),
        NULL, NULL,
        TRUE, 0,
        NULL, NULL,
        &si, &pi);


    // Now that handles have been inherited, close it to be safe.
    // You don't want to read or write to them accidentally.
    CloseHandle(hPipeOutputWrite);
    CloseHandle(hPipeInputRead);


    // Wait for CONSPAWN to finish.
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitcode;
    int result = GetExitCodeProcess(pi.hProcess, &exitcode);

    // Now test to capture DOS application output by reading
    // hPipeOutputRead.  Could also write to DOS application
    // standard input by writing to hPipeInputWrite.

    char *s = Errors.GetBuffer(10001);
    dwNumberOfBytesRead = 0;

    bTest = ReadFile(
        hPipeOutputRead,      // handle of the read end of our pipe
        s,					  // address of buffer that receives data
        10000,                // number of bytes to read
        &dwNumberOfBytesRead, // address of number of bytes read
        NULL                  // non-overlapped.
    );

    // do something with data.
    s[dwNumberOfBytesRead] = 0;  // null terminate
    Errors.ReleaseBuffer();

    // Close all remaining handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hPipeOutputRead);
    CloseHandle(hPipeInputWrite);

    if (!bTest && GetLastError() != ERROR_BROKEN_PIPE) // Note broken pipe just means there was nothing to read
    {
        wsprintf(szMsg, "Error #%d reading compiler output.", GetLastError());
        DoErrMsg(szMsg);
        return 1;
    }


    if (exitcode == 0)  // compile successful ?
    {
        CString LinkMessages;
        exitcode = LinkTI(Compiler, Name, OutFile, BoardType, Thread, LinkMessages.GetBufferSetLength(2000), 2000, MaxSize);
        LinkMessages.ReleaseBuffer();

        // Combine messages
        Errors = Errors + LinkMessages;
    }


    /*----------Console application (CONSPAWN.EXE) code------*/

    if (Err)
        strncpy(Err, Errors, MaxErrLen);

    return exitcode;
#endif
}
///-----------------------------------------------------------------------------
int SEMotionClass::LinkTI(const char * Linker, const char * Name, const char * OutFile, const int BoardType, int Thread, char * Err, int MaxErrLen, int MaxSize)
{
#if 0
    SECURITY_ATTRIBUTES sa = { 0 };
    STARTUPINFO         si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    HANDLE              hPipeOutputRead = NULL;
    HANDLE              hPipeOutputWrite = NULL;
    HANDLE              hPipeInputRead = NULL;
    HANDLE              hPipeInputWrite = NULL;
    BOOL                bTest = 0;
    DWORD               dwNumberOfBytesRead = 0;
    CHAR                szMsg[100];

    CString Errors;

    CString OFile = OutFile;

    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Create pipe for standard output redirection.
    CreatePipe(&hPipeOutputRead,  // read handle
        &hPipeOutputWrite, // write handle
        &sa,      // security attributes
        10000000      // number of bytes reserved for pipe - 0 default
    );

    // Create pipe for standard input redirection.
    CreatePipe(&hPipeInputRead,  // read handle
        &hPipeInputWrite, // write handle
        &sa,      // security attributes
        10000000      // number of bytes reserved for pipe - 0 default
    );

    // Make child process use hPipeOutputWrite as standard out,
    // and make sure it does not show on screen.
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = hPipeInputRead;
    si.hStdOutput = hPipeOutputWrite;
    si.hStdError = hPipeOutputWrite;

    CString cmd;  // build command line
    cmd = (CString)Linker + " -z " + "\"" + MainPathRoot + TILINKCMD + "\"";

    CString LinkTemplate;

    FILE *f = fopen(MainPathRoot + LINKTEMPLATE, "rt");

    if (!f)
    {
        DoErrMsg("Unable to open TI Link Template.\n\n" + MainPathRoot + LINKTEMPLATE);
        return 1;
    }

    int n = fread_s(LinkTemplate.GetBufferSetLength(100000), 100000, 1, 100000, f);
    LinkTemplate.ReleaseBufferSetLength(n);
    fclose(f);


    CString MapFile, ObjFile = Name;
    n = ObjFile.ReverseFind('.');
    if (n > 0) ObjFile.Delete(n, ObjFile.GetLength() - n);
    MapFile = ObjFile + ".map";
    ObjFile = ObjFile + ".obj";


    // Find IRAM_END in symbols file
    f = fopen(MainPathRoot + SYMBOLS, "rt");
    if (!f)
    {
        DoErrMsg("Error unable to open TI Link Template.\n\n" + MainPathRoot + SYMBOLS);
        return 1;
    }
    CString Symbols;
    n = fread_s(Symbols.GetBufferSetLength(1000), 1000, 1, 1000, f);
    Symbols.ReleaseBufferSetLength(n);
    fclose(f);

    int i0 = Symbols.Find("IRAM_END=0x");
    if (i0 == -1)
    {
        DoErrMsg("Error unable to find IRAM_END sysmbol to determine IRAM size");
        return 1;
    }

    int i1 = Symbols.Find(';',i0+11);
    if (i1 == -1)
    {
        DoErrMsg("Error unable to find IRAM_END sysmbol to determine IRAM size");
        return 1;
    }

    CString IramEnd = Symbols.Mid(i0+11, i1 - i0);
    int iRamEnd;
    int r = sscanf(IramEnd, "%x", &iRamEnd);
    if (r != 1)
    {
        DoErrMsg("Error unable to find IRAM_END sysmbol to determine IRAM size");
        return 1;
    }

    CString IR,IRL,TS,TL;
    IR.Format("0x%08x", iRamEnd);
    IRL.Format("0x%08x", 0x10020000-iRamEnd);
    TS.Format("0x%08x", GetLoadAddress(Thread, BoardType));
    TL.Format("0x%08x", MaxSize);

    LinkTemplate.Replace("{OBJECTFILE}", "\"" + ObjFile + "\"");
    LinkTemplate.Replace("{MAPFILE}", "\"" + MapFile + "\"");
    LinkTemplate.Replace("{OUTPUTFILE}", "\"" + (CString)OutFile + "\"");
    LinkTemplate.Replace("{IRAMSTART}", IR);
    LinkTemplate.Replace("{IRAMLENGTH}", IRL);
    LinkTemplate.Replace("{THREADSTART}", TS);
    LinkTemplate.Replace("{THREADLENGTH}", TL);
    LinkTemplate.Replace("{DSP_KFLOP_PATH}", MainPathRoot + "\\DSP_KFLOP\\");

    f = fopen(MainPathRoot + TILINKCMD, "wt");

    if (!f)
    {
        DoErrMsg("Unable to open TI Link Command File" + MainPathRoot + TILINKCMD);
        return 1;
    }

    n = fwrite(LinkTemplate,LinkTemplate.GetLength(), 1, f);
    fclose(f);

    CreateProcess(
        NULL,
        cmd.GetBuffer(0),
        NULL, NULL,
        TRUE, 0,
        NULL, NULL,
        &si, &pi);


    // Now that handles have been inherited, close it to be safe.
    // You don't want to read or write to them accidentally.
    CloseHandle(hPipeOutputWrite);
    CloseHandle(hPipeInputRead);


    // Wait for CONSPAWN to finish.
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitcode;
    int result = GetExitCodeProcess(pi.hProcess, &exitcode);

    // Now test to capture DOS application output by reading
    // hPipeOutputRead.  Could also write to DOS application
    // standard input by writing to hPipeInputWrite.

    char *s = Errors.GetBuffer(10001);

    bTest = ReadFile(
        hPipeOutputRead,      // handle of the read end of our pipe
        s,					  // address of buffer that receives data
        10000,                // number of bytes to read
        &dwNumberOfBytesRead, // address of number of bytes read
        NULL                  // non-overlapped.
    );


    if (!bTest)
    {
        wsprintf(szMsg, "Error #%d reading compiler output.", GetLastError());
        DoErrMsg(szMsg);
        return 1;
    }

    // do something with data.
    s[dwNumberOfBytesRead] = 0;  // null terminate
    Errors.ReleaseBuffer();

    // Close all remaining handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hPipeOutputRead);
    CloseHandle(hPipeInputWrite);

    /*----------Console application (CONSPAWN.EXE) code------*/

    if (Err)
        strncpy(Err, Errors, MaxErrLen);

    return exitcode;
#endif
}
///-----------------------------------------------------------------------------
int SEMotionClass::ValidateC(const char *Name, char *Err, int MaxErrLen)
{
#if 0
    SECURITY_ATTRIBUTES sa = { 0 };
    STARTUPINFO         si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    HANDLE              hPipeOutputRead = NULL;
    HANDLE              hPipeOutputWrite = NULL;
    HANDLE              hPipeInputRead = NULL;
    HANDLE              hPipeInputWrite = NULL;
    BOOL                bTest = 0;
    DWORD               dwNumberOfBytesRead = 0;
    CHAR                szMsg[100];



    // Try and locate splint

    CString Errors;
    CString Compiler = MainPathRoot + VALIDATOR;

    FILE *f = fopen(Compiler, "r");  // try where the KMotionDLL was irst

    if (f == NULL)
    {
        DoErrMsg("Error Locating Cppcheck.exe code validator");
        return 1;
    }
    fclose(f);


    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;




    // Create pipe for standard output redirection.
    CreatePipe(&hPipeOutputRead,  // read handle
        &hPipeOutputWrite, // write handle
        &sa,      // security attributes
        1000000   // number of bytes reserved for pipe - 0 default
    );

    // Create pipe for standard input redirection.
    CreatePipe(&hPipeInputRead,  // read handle
        &hPipeInputWrite, // write handle
        &sa,      // security attributes
        1000000   // number of bytes reserved for pipe - 0 default
    );

    // Make child process use hPipeOutputWrite as standard out,
    // and make sure it does not show on screen.
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = hPipeInputRead;
    si.hStdOutput = hPipeOutputWrite;
    si.hStdError = hPipeOutputWrite;

    CString cmd;  // build command line
    CString BindTo, IncSrcPath1, IncSrcPath2, Susppress, opts, templ;

    IncSrcPath1 = "-I \"" + MainPathRoot + "\\DSP_KFLOP\" ";
    IncSrcPath2 = "-I \"" + ExtractPath(Name) + "\" ";
    Susppress = "--suppressions-list=\"" + MainPathRoot + VALIDATOR_SUPPRESS + "\" ";
    opts = "--enable=all ";
    templ = "--template=\"{file}, line {line} : {severity} {id}\n{message}\n{code}\n\" ";

    cmd = Compiler + " \"" + Name + "\" " + opts + IncSrcPath1 + IncSrcPath2 + Susppress + templ;

    CreateProcess(
        NULL,
        cmd.GetBuffer(0),
        NULL, NULL,
        TRUE, 0,
        NULL, NULL,
        &si, &pi);


    // Now that handles have been inherited, close it to be safe.
    // You don't want to read or write to them accidentally.
    CloseHandle(hPipeOutputWrite);
    CloseHandle(hPipeInputRead);


    bool Done = true;
    DWORD Timeout;

    do
    {
        // Wait for CONSPAWN to finish.
        Timeout = WaitForSingleObject(pi.hProcess, 3000);  // timeout in 30 seconds

        Done = Timeout == WAIT_OBJECT_0 || AfxMessageBox("splint taking a long time for validation.  Continue waiting?", MB_YESNO) == IDNO;
    } while (!Done);

    DWORD exitcode;
    int result = GetExitCodeProcess(pi.hProcess, &exitcode);

    if (Timeout != WAIT_OBJECT_0)
    {
        exitcode = 0; // treat stop waiting as success
        TerminateProcess(pi.hProcess, 9999);
    }

    // Now test to capture DOS application output by reading
    // hPipeOutputRead.  Could also write to DOS application
    // standard input by writing to hPipeInputWrite.

    char *s = Errors.GetBuffer(10001);

    bTest = ReadFile(
        hPipeOutputRead,      // handle of the read end of our pipe
        s,					  // address of buffer that receives data
        10000,                // number of bytes to read
        &dwNumberOfBytesRead, // address of number of bytes read
        NULL                  // non-overlapped.
    );


    if (!bTest)
    {
        wsprintf(szMsg, "Error #%d reading Validation output.", GetLastError());
        DoErrMsg(szMsg);
        return 1;
    }

    // do something with data.
    s[dwNumberOfBytesRead] = 0;  // null terminate
    Errors.ReleaseBuffer();

    if (exitcode != 0)
    {
        Errors = "Validation Failed!\n" + Errors;
    }


    if (Timeout != WAIT_OBJECT_0) Errors = Errors + " Validation timed out";



    // Close all remaining handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hPipeOutputRead);
    CloseHandle(hPipeInputWrite);

    /*----------Console application (CONSPAWN.EXE) code------*/

    if (Err)
        strncpy(Err, Errors, MaxErrLen);

    return exitcode;
#endif
}
///-----------------------------------------------------------------------------
void SEMotionClass::ConvertToOut(int thread, const char *InFile, char *OutFile, int MaxLength)
{
#if 0
    CString OFile;
    CString IFile=InFile;

    CString InFileWithCase=InFile;
    CString ThreadString;

    if (thread>0)
    {

        ThreadString.Format("(%d).out",thread);

        IFile.MakeLower();

        if (IFile.Right(2)= ".c")
        {
            OFile = InFileWithCase.Left(InFileWithCase.GetLength()-2);
        }
        else if (IFile.Find(".txt")!=-1)
        {
            OFile = InFileWithCase.Left(InFileWithCase.GetLength()-4);
        }
        else if (IFile.Find(".cpp")!=-1)
        {
            OFile = InFileWithCase.Left(InFileWithCase.GetLength()-4);
        }
        else
        {
            OFile = InFileWithCase;
        }

        OFile += ThreadString;
    }

    strncpy(OutFile,OFile,MaxLength);
#endif
}
///-----------------------------------------------------------------------------
/// return size of coff file sections
int SEMotionClass::CheckCoffSize(const char *InFile, int *size_text, int *size_bss, int *size_data, int *size_total)
{
#if 0
    FILE *f;
    unsigned int str_size;
    char *Coff_str_table, *name;
    int i,k;
    syment csym;
    char name2[9];
    unsigned int start_text=0, end_text=0, start_bss=0, end_bss=0, start_data=0, end_data=0;
    unsigned int min,max;


    f = fopen(InFile,"rb");

    if (!f) return 1;

    if (fread(&file_hdr, FILHSZ, 1, f) != 1) return 1;

    if (fread(&o_filehdr, sizeof(o_filehdr), 1, f) != 1) return 1;

    // first read the string table

    if (fseek(f,file_hdr.f_symptr + file_hdr.f_nsyms * SYMESZ,SEEK_SET)) return 1;
    if (fread(&str_size, sizeof(int), 1, f) != 1) return 1;

    Coff_str_table = (char *)malloc(str_size);

    if (fread(Coff_str_table, str_size-4, 1, f) != 1) {free(Coff_str_table); return 1;};

    // read/process all the symbols

    // seek back to symbols

    if (fseek(f,file_hdr.f_symptr,SEEK_SET)) {free(Coff_str_table); return 1;};

    for (i=0; i< file_hdr.f_nsyms; i++)
    {
        if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); return 1;};

        if (csym._n._n_n._n_zeroes == 0)
        {
            name = Coff_str_table + csym._n._n_n._n_offset - 4 ;
        }
        else
        {
            name = csym._n._n_name;

            if (name[7] != 0)
            {
                for (k=0; k<8; k++)
                    name2[k] = name[k];

                name2[8]=0;

                name = name2;
            }
        }

        // check for the names we are looking for

        if (strcmp("__start_.text",name)==0)  start_text = csym.n_value;
        if (strcmp("__stop_.text" ,name)==0)  end_text   = csym.n_value;
        if (strcmp("__start_.bss" ,name)==0)  start_bss  = csym.n_value;
        if (strcmp("__stop_.bss"  ,name)==0)  end_bss    = csym.n_value;
        if (strcmp("__start_.data",name)==0)  start_data = csym.n_value;
        if (strcmp("__stop_.data" ,name)==0)  end_data   = csym.n_value;

        // skip any aux records

        if (csym.n_numaux == 1)
        {
            if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); return 1;};
            i++;
        }
    }

    fclose(f);
    free(Coff_str_table);

    *size_text = end_text-start_text;
    *size_bss  = end_bss -start_bss;
    *size_data = end_data-start_data;


    min = 0xffffffff;
    if (start_text != 0 && min > start_text) min = start_text;
    if (start_bss  != 0 && min > start_bss ) min = start_bss;
    if (start_data != 0 && min > start_data) min = start_data;

    max = 0x0;
    if (end_text != 0 && max < end_text) max = end_text;
    if (end_bss  != 0 && max < end_bss ) max = end_bss;
    if (end_data != 0 && max < end_data) max = end_data;

    *size_total = max-min;
#endif
    return 0;
}
///-----------------------------------------------------------------------------
/// Find special version string that is pointed to by symbol
///
/// VersionAndBuildTime
///
/// within the  .const section of the COFF file
///
#if 0
int SEMotionClass::ExtractCoffVersionString(const char *InFile, char *Version)
{
    FILE *f;
    unsigned int str_size;
    char *Coff_str_table, *name;
    int i,k;
    syment csym;
    char name2[9];
    unsigned int VersionAddress=0;
    SCNHDR sect_hdr;
    FILHDR  file_hdr;                       /* FILE HEADER STRUCTURE              */
    AOUTHDR o_filehdr;                      /* OPTIONAL (A.OUT) FILE HEADER       */


    f = fopen(InFile,"rb");

    if (!f) return 1;

    if (fread(&file_hdr, FILHSZ, 1, f) != 1) return 1;

    if (fread(&o_filehdr, sizeof(o_filehdr), 1, f) != 1) return 1;

    // search for the .const section header

    for (i=0; i<file_hdr.f_nscns; i++)
    {
        if (fread(&sect_hdr, SCNHSZ, 1, f) != 1) return 1;

        if (strcmp(".const",sect_hdr.s_name)==0) break;  // found it?
    }

    if (i==file_hdr.f_nscns) return 1;


    // now read the string table

    if (fseek(f,file_hdr.f_symptr + file_hdr.f_nsyms * SYMESZ,SEEK_SET)) return 1;
    if (fread(&str_size, sizeof(int), 1, f) != 1) return 1;

    Coff_str_table = (char *)malloc(str_size);

    if (fread(Coff_str_table, str_size-4, 1, f) != 1) {free(Coff_str_table); return 1;};

    // read/process all the symbols

    // seek back to symbols

    if (fseek(f,file_hdr.f_symptr,SEEK_SET)) {free(Coff_str_table); return 1;};

    for (i=0; i< file_hdr.f_nsyms; i++)
    {
        if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); return 1;};

        if (csym._n._n_n._n_zeroes == 0)
        {
            name = Coff_str_table + csym._n._n_n._n_offset - 4 ;
        }
        else
        {
            name = csym._n._n_name;

            if (name[7] != 0)
            {
                for (k=0; k<8; k++)
                    name2[k] = name[k];

                name2[8]=0;

                name = name2;
            }
        }

        // check for the names we are looking for

        if (strcmp("_VersionAndBuildTime",name)==0)
        {
            VersionAddress = csym.n_value;
            break;
        }

        // skip any aux records

        if (csym.n_numaux == 1)
        {
            if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); return 1;};
            i++;
        }
    }


    free(Coff_str_table);

    if (VersionAddress==0) return 1;

    // compute file offset for the string

    int offset = sect_hdr.s_scnptr +         // file pointer to data
        (VersionAddress - sect_hdr.s_paddr); // plus offset - physical address


    if (fseek(f,offset,SEEK_SET)) return 1;

    for (i=0; i<80; i++)
    {
        if (fread(Version+i, sizeof(char), 1, f) != 1) return 1;
        if (Version[i]==0) break;
    }

    fclose(f);

    if (i==80) return 1;

    return 0;
}
#endif
///-----------------------------------------------------------------------------
unsigned int MotionClass::GetLoadAddress(int thread, int BoardType)
{
    if (BoardType == BOARD_TYPE_KFLOP)
        return USER_PROG_ADDRESS_KFLOP + (thread-1) * MAX_USER_PROG_SIZE_KFLOP;
    else
        return USER_PROG_ADDRESS_KMOTION + (thread-1) * MAX_USER_PROG_SIZE_KMOTION;

}

#endif
