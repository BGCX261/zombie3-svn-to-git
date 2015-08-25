#ifndef __FILELOG_HXX__
#define __FILELOG_HXX__

#include "mytypes.h"
//#include "mydll.h"
#if defined(WIN32)

#if defined(MYFILE_EXPORTS)
/* DLL export */
#ifdef EXPORT_MYFILE
#undef EXPORT_MYFILE
#endif
#define EXPORT_MYFILE __declspec(dllexport)
#else
/* EXE import */
#define EXPORT_MYFILE __declspec(dllimport)
//#define EXPORT
#endif

#pragma comment(lib,"mythread")

#else
#define EXPORT_MYFILE 
#endif

class FileLogImpl;
class EXPORT_MYFILE FileLog
{
 public:
  FileLog();
  FileLog(const string &filename);
  ~FileLog();

  void SetFileName(const string &filename);
  void SetMode(tBool mode);
  void ToggleDebugMode();

  static void SetDebugMode(tBool mode);
  static bool Init(const string &filename);
  static bool Info(const string &msg);
  static bool Error(const string &msg);
  static bool Warning(const string &msg);
  static bool Debug(const string &msg);
  static bool Fatal(const string &msg);
  static bool Log(const string &msg);

  bool AppendTrace(const string &msg);
  bool AppendInfo(const string &msg);
  bool AppendError(const string &msg);
  bool AppendWarning(const string &msg);
  bool AppendDebug(const string &msg);
  bool AppendFatal(const string &msg);
  bool AppendLog(const string &msg);
 private:
  // Copy constructor is private to FORCE the user to instantiate 
  // a new object with 'new' instead of static allocation.
  // This is because we cannot copy this object so easily 
  // mainly because of internal objects.
  FileLog(const FileLog &model);

  FileLogImpl *_impl;
};

#endif // __FILELOG_HXX__
