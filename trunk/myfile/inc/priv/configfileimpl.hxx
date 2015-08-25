#ifndef _CONFIGFILEIMPL_HXX_
#define _CONFIGFILEIMPL_HXX_

#include <map>
#include "mytypes.h"
#include "myfilelog.hxx"

using namespace std;

#if 0
struct char_less
{
  bool operator()(const char *__x, const char *__y) const
  { 
    int res;
    res=strcmp(__x,__y);
    return res<0; 
  }
};
#endif

class ConfigFileImpl
{
 public:
  // Container containing the pair (key, value).
  typedef map<string, string> tContainer;
  typedef tContainer::const_iterator tContainerCIt;

  ConfigFileImpl();
  virtual ~ConfigFileImpl();
  virtual void SetDebugMode(bool mode);
  virtual tBool Read(const string &config_file);
  virtual tBool GetValue(const string &key, string &value) const;
  virtual tBool GetValue(const string &key, tSInt32 *value) const;
  virtual void SetValue(const string &key, const string &value);
  virtual const string &Name() const;

 private:
  string _filename;
  tContainer _values;
  bool _debug_mode;
};

#endif //_CONFIGFILEIMPL_HXX_
