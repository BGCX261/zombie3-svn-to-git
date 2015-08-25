//#include "scrambled_symbols.h"
#include "mysyslog.hxx"
#include "mymutex.hxx"

// Default Application Name is 'unknown';
static string _app_name="unknown";

#if defined(linux)
// Default is to print PID of current process (LOG_PID) and to wait 
// for syslog call to actually open the syslog facility (LOG_ODELAY).
static int _log_option=LOG_PID|LOG_ODELAY;


// Default facility.
static int _facility=LOG_DAEMON;
#elif defined(WIN32)
static int _log_option=0;
static int _facility=0;
#else
#error "Unsupported platform"
#endif

// Following is a mutex that will be used to render the current
// namespace thread safe and immune to problems caused by parallel 
// thread execution.
static Mutex _critical_section_protect;

///////////////////////////////////////////////////////////////////////////////
//
//////////
void SysLog::Init(const string &app_name, int option, int facility)
{
  SetAppName(app_name);
  SetLogOption(option);
  SetFacility(facility);
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
void SysLog::SetAppName(const string &app_name)
{
  _critical_section_protect.Lock();
  _app_name=app_name;
  _critical_section_protect.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
void SysLog::SetLogOption(int option)
{
  _critical_section_protect.Lock();
  _log_option=option;
  _critical_section_protect.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
void SysLog::SetFacility(int facility)
{
  _critical_section_protect.Lock();
  _facility=facility;
  _critical_section_protect.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
void SysLog::Log(int priority, const string &msg)
{
  _critical_section_protect.Lock();
#if defined(linux)
  openlog(_app_name.c_str(),_log_option,_facility);
  syslog(priority, "%s\n", msg.c_str());
  closelog();
#elif defined(WIN32)
#else
#error "SysLog::Log(int priority, const string &msg): Function is empty."
#endif
  _critical_section_protect.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
//
//////////
void SysLog::Log(int priority, stringstream &msgstream)
{
  SysLog::Log(priority,msgstream.str());
  msgstream.clear();
}
