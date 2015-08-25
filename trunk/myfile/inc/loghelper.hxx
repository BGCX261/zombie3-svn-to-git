#ifndef _LOGHELPER_HXX_
#define _LOGHELPER_HXX_

#include <sstream>
using namespace std;

#define LOGTRACE(x)                               \
	    if (_filelog)                         \
	      {                                   \
		stringstream *ss;                 \
                ss = new stringstream();          \
		(*ss)<<x;                         \
		_filelog->AppendTrace(ss->str()); \
                delete  ss;                       \
                ss=0;                             \
	      } else             

#define LOGINFO(x)                                \
	    if (_filelog)                         \
	      {                                   \
		stringstream *ss;                 \
                ss = new stringstream();          \
		(*ss)<<x;                         \
		_filelog->AppendInfo(ss->str());  \
                delete  ss;                       \
                ss=0;                             \
	      } else             

#define LOGWARNING(x)                              \
	    if (_filelog)                          \
	      {                                    \
		stringstream *ss;                  \
                ss = new stringstream();           \
		(*ss)<<x;                          \
		_filelog->AppendWarning(ss->str());\
                delete ss;                         \
                ss=0;                              \
	      } else             

#define LOGERROR(x)                                \
	    if (_filelog)                          \
	      {                                    \
		stringstream *ss;                  \
                ss = new stringstream();           \
		(*ss)<<x;                          \
		_filelog->AppendError(ss->str());  \
                delete ss;                         \
                ss=0;                              \
	      } else             

#define LOGFATAL(x)                                \
	    if (_filelog)                          \
	      {                                    \
		stringstream *ss;                  \
                ss = new stringstream();           \
		(*ss)<<x;                          \
		_filelog->AppendFatal(ss->str());  \
                delete ss;                         \
                ss=0;                              \
	      } else             

#define LOGDEBUG(x)                                \
	    if (_filelog)                          \
	      {                                    \
		stringstream *ss;                  \
                ss = new stringstream();           \
		(*ss)<<x;                          \
		_filelog->AppendDebug(ss->str());  \
                delete ss;                         \
                ss=0;                              \
	      } else             

#endif // _LOGHELPER_HXX_
