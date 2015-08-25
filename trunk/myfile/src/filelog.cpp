//#include "scrambled_symbols.h"
#include "priv/filelogimpl.hxx"

#include "myfilelog.hxx"
#include "mymutex.hxx"

#include <assert.h>
#include <iostream>
#include <time.h>

using namespace std;

static FileLogImpl _sharedlog;

FileLogImpl::FileLogImpl()
  : _debug_mode(false), _log_file(),
    _log(), _critical_section_protect()
{
}

FileLogImpl::FileLogImpl(const string &filename)
  : _debug_mode(false), _log_file(filename),
    _log(), _critical_section_protect()
{
}

FileLogImpl::~FileLogImpl()
{
}

void FileLogImpl::SetDebugMode(tBool mode)
{
  _debug_mode=mode;
}

void FileLogImpl::SetFileName(const string &filename)
{
  _log_file=filename;
}

void FileLogImpl::ToggleDebugMode()
{
  if (_debug_mode)
    {
      _debug_mode=false;
    }
  else
    {
      _debug_mode=true;
    }
}

bool FileLogImpl::Trace(const string &msg)
{
  bool res;

  stringstream *ss;
  ss=new stringstream();

  (*ss)<<"[TRACE]   "<<msg;

  res=Log(ss->str());

  delete ss;

  return res;
}

bool FileLogImpl::Info(const string &msg)
{
  bool res;

  stringstream *ss;
  ss=new stringstream();

  (*ss)<<"[INFO]    "<<msg;

  res=Log(ss->str());

  delete ss;

  return res;
}

bool FileLogImpl::Error(const string &msg)
{
  bool res;

  stringstream *ss;
  ss=new stringstream();

  (*ss)<<"[ERROR]   "<<msg;

  res=Log(ss->str());

  delete ss;

  return res;
}

bool FileLogImpl::Warning(const string &msg)
{
  bool res;

  stringstream *ss;
  ss=new stringstream();

  (*ss)<<"[WARNING] "<<msg;

  res=Log(ss->str());

  delete ss;

  return res;
}

bool FileLogImpl::Debug(const string &msg)
{
  bool res;

  stringstream *ss;

  if (!_debug_mode) return false;

  ss=new stringstream();

  (*ss)<<"[DEBUG]   "<<msg;

  res=Log(ss->str());

  delete ss;

  return res;
}

bool FileLogImpl::Fatal(const string &msg)
{
  bool res;

  stringstream *ss;
  ss=new stringstream();

  (*ss)<<"[FATAL]   "<<msg;

  res=Log(ss->str());

  delete ss;

  return res;
}

bool FileLogImpl::Log(const string &msg)
{
  char timestring[100];
  struct tm mytm;
  time_t t;

  // Protection from multiple threads execution starts here.
  _critical_section_protect.Lock();

  // Getting current time stamp.
  time(&t);

#if defined(linux)
  localtime_r(&t,&mytm);
#elif defined(WIN32)
  mytm=*(localtime(&t));
#else
#error "FileLog::Log: Implementation partly empty due to unsupported OS"
#endif

  strftime(timestring,100,"%Y/%m/%d %H:%M:%S",&mytm);

  _log.open(_log_file.c_str(),ios::out|ios::app);

  if (!_log)
    {
      // Path to file is whether not existing or write access is not allowed.
      _critical_section_protect.Unlock();
      return false;
    }

  _log<<timestring<<" "<<msg;

  // Adding/forcing a CR/LF if not present.
  if (msg[msg.length()-1]!='\n') _log<<'\n';

  _log.flush();

  _log.close();

  // Protection from multiple threads execution ends here.
  _critical_section_protect.Unlock();

  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
FileLog::FileLog(const string &filename)
{
  _impl=new FileLogImpl(filename);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
FileLog::FileLog()
{
  _impl=new FileLogImpl();
}

FileLog::FileLog(const FileLog &model)
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"FileLog::FileLog(const FileLog &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif

}

///////////////////////////////////////////////////////////////////////////////
//
//////////
FileLog::~FileLog()
{
  delete _impl;
  _impl=0;
}

void FileLog::SetFileName(const string &filename)
{
  _impl->SetFileName(filename);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
void FileLog::SetDebugMode(bool mode)
{
  _sharedlog.SetDebugMode(mode);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
void FileLog::ToggleDebugMode()
{
  _impl->ToggleDebugMode();
}

bool FileLog::Init(const string &filename)
{
  _sharedlog.SetFileName(filename);

  return true;
}

void FileLog::SetMode(tBool mode)
{
  _impl->SetDebugMode(mode);
}

bool FileLog::Info(const string &msg)
{
  return _sharedlog.Info(msg);
}

bool FileLog::Error(const string &msg)
{
  return _sharedlog.Error(msg);
}

bool FileLog::Warning(const string &msg)
{
  return _sharedlog.Warning(msg);
}

bool FileLog::Debug(const string &msg)
{
  return _sharedlog.Debug(msg);
}

bool FileLog::Fatal(const string &msg)
{
  return _sharedlog.Fatal(msg);
}

bool FileLog::Log(const string &msg)
{
  return _sharedlog.Log(msg);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
bool FileLog::AppendTrace(const string &msg)
{
  return _impl->Trace(msg);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
bool FileLog::AppendInfo(const string &msg)
{
  return _impl->Info(msg);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
bool FileLog::AppendError(const string &msg)
{
  return _impl->Error(msg);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
bool FileLog::AppendWarning(const string &msg)
{
  return _impl->Warning(msg);
}


///////////////////////////////////////////////////////////////////////////////
//
//////////
bool FileLog::AppendDebug(const string &msg)
{
  return _impl->Debug(msg);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
bool FileLog::AppendFatal(const string &msg)
{
  return _impl->Fatal(msg);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
bool FileLog::AppendLog(const string &msg)
{
  return _impl->Log(msg);
}

