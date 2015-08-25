#ifndef __SYSLOG_HXX__
#define __SYSLOG_HXX__

// Including syslog.h in order to get the definition of 
// the constants like LOG_ERR, or LOG_INFO or LOG_DAEMON,
// etc ... .
#if defined(linux)
#include <syslog.h>
#elif defined(WIN32)
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */
#else
#error "Unsupported platform"
#endif

#include "mydll.h"
#include <string>
#include <sstream>
using namespace std;

namespace SysLog
{
  void Init(const string &app_name, int option, int facility);
  void SetAppName(const string &app_name);
  void SetLogOption(int option);
  void SetFacility(int facility);
  void Log(int priority, const string &msg);
  void Log(int priority, stringstream &msgstream);
}

#endif // __SYSLOG_HXX__
