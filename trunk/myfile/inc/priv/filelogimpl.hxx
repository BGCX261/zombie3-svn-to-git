#ifndef _FILELOGIMPL_HXX_
#define _FILELOGIMPL_HXX_

#include "mytypes.h"
#include "mymutex.hxx"

#include <fstream>
#include <sstream>
using namespace std;

class FileLogImpl
{
 public:
  FileLogImpl(const string &filename);
  FileLogImpl();
  ~FileLogImpl();

  void SetFileName(const string &filename);
  void SetDebugMode(tBool mode);
  void ToggleDebugMode();

  bool Trace(const string &msg);
  bool Info(const string &msg);
  bool Error(const string &msg);
  bool Warning(const string &msg);
  bool Debug(const string &msg);
  bool Fatal(const string &msg);
  bool Log(const string &msg);
 private:
  
  // If _debug_mode is true, then all subsequent calls to 
  // 'Debug' function will be actually logged into the file.
  // If not true, all calls to 'Debug' function will be dismissed.
  bool _debug_mode;

  // File name to be used for logging purpose.
  string _log_file;

  ofstream _log;

  // Following is a mutex that will be used to render the current
  // namespace thread safe and immune to problems caused by parallel 
  // thread execution.
  Mutex _critical_section_protect;

};

#endif // _FILELOGIMPL_HXX_
