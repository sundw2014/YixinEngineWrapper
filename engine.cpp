#include "engine.h"
#include <iostream>
#include <sstream>
#include <string>

Engine::Engine(int w, int h, TCHAR engineCmd[])
{
  boardsizew=w;
  boardsizeh=h;
  loadEngine(engineCmd);
}
Engine::~Engine()
{

}
bool Engine::loadEngine(TCHAR engineCmd[])
{
  SECURITY_ATTRIBUTES saAttr;

// Set the bInheritHandle flag so pipe handles are inherited.

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

// Create a pipe for the child process's STDOUT.

  if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
     ErrorExit(TEXT("StdoutRd CreatePipe"));

// Ensure the read handle to the pipe for STDOUT is not inherited.

  if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
     ErrorExit(TEXT("Stdout SetHandleInformation"));

// Create a pipe for the child process's STDIN.

  if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
     ErrorExit(TEXT("Stdin CreatePipe"));

// Ensure the write handle to the pipe for STDIN is not inherited.

  if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
     ErrorExit(TEXT("Stdin SetHandleInformation"));

  // Create a child process that uses the previously created pipes for STDIN and STDOUT.
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  BOOL bSuccess = FALSE;

  // Set up members of the PROCESS_INFORMATION structure.

  ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

  // Set up members of the STARTUPINFO structure.
  // This structure specifies the STDIN and STDOUT handles for redirection.

  ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = g_hChildStd_OUT_Wr;
  siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
  siStartInfo.hStdInput = g_hChildStd_IN_Rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  // Create the child process.

  bSuccess = CreateProcess(NULL,
    engineCmd,     // command line
    NULL,          // process security attributes
    NULL,          // primary thread security attributes
    TRUE,          // handles are inherited
    0,             // creation flags
    NULL,          // use parent's environment
    NULL,          // use parent's current directory
    &siStartInfo,  // STARTUPINFO pointer
    &piProcInfo);  // receives PROCESS_INFORMATION

    // If an error occurs, exit the application.
    if ( ! bSuccess )
      ErrorExit(TEXT("CreateProcess"));
    else
    {
      // Close handles to the child process and its primary thread.
      // Some applications might keep these handles to monitor the status
      // of the child process, for example.

      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
	}
	return true;
}

bool Engine::writeCommand(const std::string &_command)
{
  DWORD dwWritten;
  BOOL bSuccess = FALSE;

  bSuccess = WriteFile(g_hChildStd_IN_Wr, _command.c_str(), _command.length(), &dwWritten, NULL);

  return(bSuccess);
}

bool Engine::getBest(int best[])
{
    // Read output from the child process's pipe for STDOUT
       DWORD dwRead;
       CHAR chBuf[_ENGINE_BUFSIZE];
       BOOL bSuccess = FALSE;

       size_t count=0;

       for (;;)
       {
          bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf+count, _ENGINE_BUFSIZE, &dwRead, NULL);

          if(!bSuccess || dwRead==0)
            continue;

          count += dwRead;
          chBuf[count] = '\0';
		  std::string result; result += chBuf;

          size_t index_of_car=0;
          bool isFull = true;
          size_t expectLine=6;

          for(int i=0;i<expectLine;i++)
          {
              if((index_of_car=result.find('\n',index_of_car)) == std::string::npos)
              {
                  isFull = false;
                  break;
              }
              index_of_car+=1;
          }

          //测试行数不足继续接收
          if(!isFull)
            continue;

         //测试接收完毕开始解析

         std::string line;
         std::istringstream is(result);

         for(int i=0;i<expectLine-1;i++)
          std::getline(is,line);
		 char temp_c;
         is>>best[0]>>temp_c>>best[1];
		 break;
      }
}
bool Engine::getBest_async(int best[],void *callBack)
{
	while(1);
}
void Engine::ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box,
// and exit from the application.
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(1);
}
