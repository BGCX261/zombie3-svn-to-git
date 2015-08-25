#ifndef __MUTEXT_HXX__
#define __MUTEXT_HXX__

#include "mythread.hxx"

class MutexImpl;

class EXPORT_MYTHREAD Mutex
{ 
 public:
  Mutex();
  ~Mutex();
  void Lock(void);
  void Unlock(void);
  
private:
  Mutex(const Mutex &model);

  // Implemtation of Mutex is hidden to user
  MutexImpl *_impl;
};

#endif // __MUTEXT_HXX__
