#include "myevent.hxx"
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

class EventImpl
{ 
 public:

#if defined(linux)
  //typedef pthread_mutex_t tMutex;
	 #error "Not Implemented yes."
#elif defined(WIN32)
  typedef HANDLE tEvent;
#endif

  EventImpl();
  ~EventImpl();
  void SetEvent(void);
  void WaitEvent(void);
  
 protected:
  tEvent _event;
};

///////////////////////////////////////////////////////////////////////////////
//
//
EventImpl::EventImpl()
  : _event()
{
#if defined(linux)
  //pthread_mutexattr_t attr;
  //pthread_mutexattr_init(&attr);
  //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
  //pthread_mutex_init(&_event,&attr);
  //pthread_mutexattr_destroy(&attr);
	#error "Not Implemented yes."

#elif defined(WIN32)
  _event = CreateEvent( 
        NULL,         // default security attributes
        FALSE,        // auto-reset event
        TRUE,         // initial state is signaled
        NULL		  // object name
        ); 

#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
EventImpl::~EventImpl()
{
#if defined(linux)
  //pthread_mutex_destroy(&_event);
	#error "Not Implemented yes."
#elif defined(WIN32)
  CloseHandle(_event);
#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
void EventImpl::WaitEvent(void)
{
#if defined(linux)
  //pthread_mutex_lock(&_event);
	#error "Not Implemented yes."
#elif defined(WIN32)
  WaitForSingleObject(_event, INFINITE );
#endif
}
///////////////////////////////////////////////////////////////////////////////
//
//
void EventImpl::SetEvent(void)
{
#if defined(linux)
  //pthread_mutex_unlock(&_event);
	#error "Not Implemented yes."
#elif defined(WIN32)
	::SetEvent(_event);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
Event::Event()
  : _impl(0)
{
  _impl=new EventImpl();
}

Event::Event(const Event &model)
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"Event::Event(const Event &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//
Event::~Event()
{
  if (_impl) delete _impl;
  _impl=0;
}
///////////////////////////////////////////////////////////////////////////////
//
//
void Event::WaitEvent(void)
{
  _impl->WaitEvent();
}
///////////////////////////////////////////////////////////////////////////////
//
//
void Event::SetEvent(void)
{
  _impl->SetEvent();
}
  
