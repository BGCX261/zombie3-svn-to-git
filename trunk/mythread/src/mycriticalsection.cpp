#include "mycriticalsection.hxx"
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
#include <assert.h>

#else
//#error "Unknown Operating System"
#endif

using namespace std;

class CriticalSectionImpl
{ 
 public:

#if defined(linux)
  //typedef pthread_mutex_t tMutex;
  // #error "Not Implemented yes."
#elif defined(WIN32)
  typedef LPCRITICAL_SECTION tCriticalSection;
#endif

  CriticalSectionImpl();
  ~CriticalSectionImpl();

  void EnterCriticalSection(void);
  void LeaveCriticalSection(void);
  bool TryEnterCriticalSection(void);
  
 protected:
  tCriticalSection _criticalsection;
};

///////////////////////////////////////////////////////////////////////////////
//
//
CriticalSectionImpl::CriticalSectionImpl()
  : _criticalsection(NULL)
{
#if defined(linux)
  //pthread_mutexattr_t attr;
  //pthread_mutexattr_init(&attr);
  //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
  //pthread_mutex_init(&_criticalsection,&attr);
  //pthread_mutexattr_destroy(&attr);
  //#error "Not Implemented yes."

#elif defined(WIN32)
 _criticalsection = new CRITICAL_SECTION;
  InitializeCriticalSection( _criticalsection); 

#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
CriticalSectionImpl::~CriticalSectionImpl()
{
#if defined(linux)
  //pthread_mutex_destroy(&_criticalsection);
  //#error "Not Implemented yes."
#elif defined(WIN32)
  DeleteCriticalSection(_criticalsection);
  delete _criticalsection;
#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
void CriticalSectionImpl::EnterCriticalSection(void)
{
#if defined(linux)
  //pthread_mutex_lock(&_criticalsection);
  //#error "Not Implemented yes."
#elif defined(WIN32)
	::EnterCriticalSection(_criticalsection);
#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
void CriticalSectionImpl::LeaveCriticalSection(void)
{
#if defined(linux)
  //pthread_mutex_unlock(&_criticalsection);
  //#error "Not Implemented yes."
#elif defined(WIN32)
	::LeaveCriticalSection(_criticalsection);
#endif
}

bool CriticalSectionImpl::TryEnterCriticalSection(void)
{
#if defined(linux)
  //pthread_mutex_unlock(&_criticalsection);
  //#error "Not Implemented yes."
#elif defined(WIN32)
	//::TryEnterCriticalSection(_criticalsection);
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//
CriticalSection::CriticalSection()
  : _impl(0)
{
  _impl=new CriticalSectionImpl();
}

CriticalSection::CriticalSection(const CriticalSection &model)
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"CriticalSection::CriticalSection(const CriticalSection &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//
CriticalSection::~CriticalSection()
{
  if (_impl) delete _impl;
  _impl=0;
}
///////////////////////////////////////////////////////////////////////////////
//
//
void CriticalSection::EnterCriticalSection(void)
{
  _impl->EnterCriticalSection();
}
///////////////////////////////////////////////////////////////////////////////
//
//
void CriticalSection::LeaveCriticalSection(void)
{
  _impl->LeaveCriticalSection();
}
bool CriticalSection::TryEnterCriticalSection(void)
{
  return _impl->TryEnterCriticalSection();
}