#ifndef __configfile_hxx__
#define __configfile_hxx__

#include "mytypes.h"
//#include "mydll.h"
#include "myfilelog.hxx"

class ConfigFileImpl;
class EXPORT_MYFILE ConfigFile
{
 public:
  ConfigFile();
  virtual ~ConfigFile();
  virtual void SetDebugMode(bool mode);
  virtual tBool Read(const string &config_file);
  virtual tBool GetValue(const string &key, string &value) const; 
  virtual tBool GetValue(const string &key, tSInt32 *value) const;
  virtual void SetValue(const string &key, const string &value);
  virtual const string &Name() const;
 private:
  // Copy constructor is private to FORCE the user to instantiate 
  // a new object with 'new' instead of static allocation.
  // This is because we cannot copy this object so easily 
  // mainly because of internal objects.
  ConfigFile(const ConfigFile &model);
  ConfigFileImpl *_impl;
};

#endif // __configfile_hxx__
