#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <string>

#define _ENGINE_BUFSIZE 4096

class Engine
{
private:
  HANDLE g_hChildStd_IN_Rd;
  HANDLE g_hChildStd_IN_Wr;
  HANDLE g_hChildStd_OUT_Rd;
  HANDLE g_hChildStd_OUT_Wr;
  int boardsizew,boardsizeh;

  bool loadEngine(TCHAR engineCmd[]);
  void ErrorExit(PTSTR);
public:
  bool writeCommand(const std::string &_command);
  bool getBest(int best[]);
  bool getBest_async(int best[],void *callBack);
  bool flushStdOut()
  {
	  FlushFileBuffers(g_hChildStd_OUT_Rd);
	  return true;
  }
  Engine(int w, int h, TCHAR engineCmd[]);
  ~Engine();
};

#endif
