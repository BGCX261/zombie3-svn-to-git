#include "mymutex.hxx"
#include <iostream>

#if defined(linux)
// Found in /usr/include
#include <pthread.h>
#elif defined(WIN32)
/**
 * to invalidate the winsock.h in windows.h file
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>
#else
#error "Unknown Operating System"
#endif

#include <assert.h>

using namespace std;

class MutexImpl
{ 
 public:

#if defined(linux)
  typedef pthread_mutex_t tMutex;
#elif defined(WIN32)
  typedef HANDLE tMutex;
#endif

  MutexImpl();
  ~MutexImpl();
  void Lock(void);
  void Unlock(void);
  
 protected:
  tMutex _mutex;
};

///////////////////////////////////////////////////////////////////////////////
//
//
MutexImpl::MutexImpl()
  : _mutex()
{
#if defined(linux)
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init(&_mutex,&attr);
  pthread_mutexattr_destroy(&attr);

#elif defined(WIN32)
  _mutex=CreateMutex(NULL, FALSE, NULL );
#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
MutexImpl::~MutexImpl()
{
#if defined(linux)
  pthread_mutex_destroy(&_mutex);
#elif defined(WIN32)
  CloseHandle(_mutex);
#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
void MutexImpl::Lock(void)
{
#if defined(linux)
  pthread_mutex_lock(&_mutex);
#elif defined(WIN32)
  WaitForSingleObject(_mutex, INFINITE );
#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
void MutexImpl::Unlock(void)
{
#if defined(linux)
  pthread_mutex_unlock(&_mutex);
#elif defined(WIN32)
  ReleaseMutex(_mutex);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
Mutex::Mutex()
  : _impl(0)
{
  _impl=new MutexImpl();
}

Mutex::Mutex(const Mutex &model)
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"Mutex::Mutex(const Mutex &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//
Mutex::~Mutex()
{
  if (_impl) delete _impl;
  _impl=0;
}
///////////////////////////////////////////////////////////////////////////////
//
//
void Mutex::Lock(void)
{
  _impl->Lock();
}
///////////////////////////////////////////////////////////////////////////////
//
//
void Mutex::Unlock(void)
{
  _impl->Unlock();
}
  
