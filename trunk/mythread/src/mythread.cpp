#include "mythread.hxx"
#include "mymutex.hxx"

#include <string>
#include <iostream>
#include <sstream>

#include <assert.h>

#if defined(linux)
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(WIN32)
#else
#error "Unknown Operating System"
#endif
using namespace std;

#if defined(linux)
static int return_value=0;
#endif

class ThreadImpl
{
  friend class Thread;
 public:
  typedef Thread::ThreadId  ThreadId;
  typedef Thread::Arg Arg;
  typedef Thread::tState tState;
  
  ThreadImpl();
  virtual ~ThreadImpl();
  ThreadId Id();
  unsigned long RealThreadId();
  Arg Argument();
  tState State();
  void Start(Thread *parent, Arg a=NULL);
  virtual void Join();
  void Resume();

 protected:
  void Run(Arg arg);
  void Wait(unsigned int seconds);
  void WaitMs(unsigned int milliseconds);
  void Stop();
  void Terminate();

#if defined(linux)
  static void *EntryPoint(void * pthis);
#elif defined(WIN32)
  static unsigned __stdcall EntryPoint(void * pthis);

#endif

 private:
  Arg      _arg;
  ThreadId _id;
  unsigned long _real_threadid;
  tState   _state;
  Mutex    _arg_mutex;    // This mutex is necessary to properly manage the 
                          // _state attributes in a multi threaded context .

  Mutex    _id_mutex;     // This mutex is necessary to properly manage the 
                          // _state attributes in a multi threaded context .

  Mutex    _state_mutex;  // This mutex is necessary to properly manage the 
                          // _state attributes in a multi threaded context .
};

///////////////////////////////////////////////////////////////////////////////
//
ThreadException::ThreadException(const string &s)
  : _msg("ThreadException: "+s)
{
}

///////////////////////////////////////////////////////////////////////////////
//
ThreadException::~ThreadException()
{
}

///////////////////////////////////////////////////////////////////////////////
//
const string &ThreadException::Description() const
{
  return(_msg);
}

///////////////////////////////////////////////////////////////////////////////
//
ostream &operator<<(ostream &os, const ThreadException &e)
{
  return(os<<e.Description());
}


///////////////////////////////////////////////////////////////////////////////
//
ThreadImpl::ThreadImpl()
: _arg(NULL), 
  _id((ThreadId)-1), 
  _state(Thread::INITIALIZED_STATE), 
  _arg_mutex(), 
  _id_mutex(), 
  _state_mutex()
{
}

///////////////////////////////////////////////////////////////////////////////
//
ThreadImpl::~ThreadImpl()
{
}

///////////////////////////////////////////////////////////////////////////////
//
ThreadImpl::ThreadId ThreadImpl::Id()
{
  ThreadId theid;

  if (State()==Thread::INITIALIZED_STATE) 
    {
      throw ThreadException("Thread ID not available since not yet running.");
    }

  _id_mutex.Lock();
  theid=_id;
  _id_mutex.Unlock();

  return(theid);
}
unsigned long ThreadImpl::RealThreadId()
{
  unsigned long theid;

  if (State()==Thread::INITIALIZED_STATE) 
    {
      throw ThreadException("Thread ID not available since not yet running.");
    }

  _id_mutex.Lock();
  theid=_real_threadid;
  _id_mutex.Unlock();

  return(theid);
}

///////////////////////////////////////////////////////////////////////////////
//
ThreadImpl::Arg ThreadImpl::Argument()
{
  Arg thearg;
  if (State()==Thread::INITIALIZED_STATE)
    {
      throw ThreadException("Thread Argument not available since not yet started.");
    }

  _arg_mutex.Lock();
  thearg=_arg;
  _arg_mutex.Unlock();

  return(thearg);
}

///////////////////////////////////////////////////////////////////////////////
//
ThreadImpl::tState ThreadImpl::State()
{
  tState thestate;

  _state_mutex.Lock();
  thestate=_state;
  _state_mutex.Unlock();
  
  return thestate;
}

///////////////////////////////////////////////////////////////////////////////
//
void ThreadImpl::Start(Thread *parent, Arg arg)
#if defined(linux)
{
  int rc;
  pthread_attr_t attr;
  
 if ( (State()!=Thread::INITIALIZED_STATE) && 
      (State()!=Thread::FINISHED_STATE) )
    {
      throw ThreadException("Tried to start or restart an already running thread.");
    }

  _arg_mutex.Lock();
  _arg=arg;
  _arg_mutex.Unlock();

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // Lock must occur prior the new thread creation (call of  pthread_create).
  _state_mutex.Lock();

  // Create thread in JOINABLE mode. This call update the private
  // member _id of the called object so it is possible to use the Id()
  // and Join() methods after the current function terminates..
  rc = pthread_create(&(this->_id), &attr, &ThreadImpl::EntryPoint, parent); 
  // Once created, the new thread is stopped until the following attribute
  // is updated.
  _state=Thread::RUNNING_STATE;
  _state_mutex.Unlock();

  // Free attribute before detecting eventual problem.
  pthread_attr_destroy(&attr);

  if (rc)
    {
      ostringstream ostr;
      
      ostr<<"Failed to start the thread. (pthread_create returned code is "<<rc<<").";
      throw ThreadException(ostr.str());
    }
}
#elif defined(WIN32)
{
  int rc;
  unsigned int threadID;
  
  if ( (State()!=Thread::INITIALIZED_STATE) && 
       (State()!=Thread::FINISHED_STATE) )
    {
      throw ThreadException("Tried to start or restart an already running thread.");
    }
  
  _arg_mutex.Lock();
  _arg=arg;
  _arg_mutex.Unlock();

  // Lock must occur prior the new thread creation (call of  pthread_create).
  _state_mutex.Lock();

  /* For windows, thread Id is more important */
  //rc = _beginthread(&ThreadImpl::EntryPoint,0, parent);
  //this->_id=(ThreadId) rc;
  rc = _beginthreadex( NULL, 0, &ThreadImpl::EntryPoint, parent, 0, &threadID );
  this->_id=(ThreadId) rc;
  this->_real_threadid = threadID;

  // Once created, the new thread is stopped until the following attribute
  // is updated.
  _state=Thread::RUNNING_STATE;
  _state_mutex.Unlock();

  if (rc==-1)
    {
      ostringstream ostr;
      
      ostr<<"Failed to start the thread. (_beginthread returned code is "<<errno<<").";
      throw ThreadException(ostr.str());
    }
}
#endif //ThreadImpl::Start

///////////////////////////////////////////////////////////////////////////////
//
void ThreadImpl::Join()
#if defined(linux)
{
  int rc;
  int status;

  if (Id() == (ThreadId) -1 )
    {
      // There is nothing to join. We must complain.
      ostringstream ostr;
      
      ostr<<"Attempting to join a not started thread.";
      throw ThreadException(ostr.str());
    }

  rc = pthread_join(Id(),(void **) &status);
  if (rc)
    {
      ostringstream ostr;
      
      ostr<<"Attempting to join thread "<<Id()<<" but failed on error "<<rc<<".";
      throw ThreadException(ostr.str());
    }
}
#elif defined(WIN32)
{
  unsigned int rc;
  if (Id() == (ThreadId) -1 )
    {
      // There is nothing to join. We must complain.
      ostringstream ostr;
      
      ostr<<"Attempting to join a not started thread.";
      throw ThreadException(ostr.str());
    }

  rc=WaitForSingleObject(Id(), INFINITE);
  if (rc==WAIT_FAILED)
    {
      ostringstream ostr;
      
      ostr<<"Attempting to join thread "<<Id()<<" but failed on error "<<GetLastError()<<".";
      throw ThreadException(ostr.str());
    }
}
#endif // void ThreadImpl::Join()

///////////////////////////////////////////////////////////////////////////////
//
void ThreadImpl::Resume()
#if defined(linux)
{
  if (State()!=Thread::STOPPED_STATE) 
    {
      throw ThreadException("Tried to resume a not stopped thread.");
    }

  _state_mutex.Lock();
  _state=Thread::RUNNING_STATE;
  _state_mutex.Unlock();
}
#elif defined(WIN32)
{
  if (State()!=Thread::STOPPED_STATE) 
    {
      throw ThreadException("Tried to resume a not stopped thread.");
    }

  ResumeThread(Id());

  _state_mutex.Lock();
  _state=Thread::RUNNING_STATE;
  _state_mutex.Unlock();
}
#endif // void ThreadImpl::Resume()

///////////////////////////////////////////////////////////////////////////////
//
void ThreadImpl::Wait(unsigned int seconds)
{
  _state_mutex.Lock();
  _state=Thread::SLEEPING_STATE;
  _state_mutex.Unlock();

  Thread::Sleep(seconds);

  _state_mutex.Lock();
  _state=Thread::RUNNING_STATE;
  _state_mutex.Unlock();

}

///////////////////////////////////////////////////////////////////////////////
//
void ThreadImpl::WaitMs(unsigned int milliseconds)
{
  _state_mutex.Lock();
  _state=Thread::SLEEPING_STATE;
  _state_mutex.Unlock();

  Thread::SleepMs(milliseconds);

  _state_mutex.Lock();
  _state=Thread::RUNNING_STATE;
  _state_mutex.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
//
void ThreadImpl::Stop()
#if defined(linux)
{
  _state_mutex.Lock();
  _state=Thread::STOPPED_STATE;
  _state_mutex.Unlock();

  while(State()==Thread::STOPPED_STATE)
    {
	  Thread::Yield();
    }
}
#elif defined(WIN32)
{
  _state_mutex.Lock();
  _state=Thread::STOPPED_STATE;
  _state_mutex.Unlock();

  SuspendThread(Id());
}
#endif // void ThreadImpl::Stop()

///////////////////////////////////////////////////////////////////////////////
//
void ThreadImpl::Terminate()
{
  _state_mutex.Lock();
  _state=Thread::FINISHED_STATE;
  _state_mutex.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
//
#if defined(linux)
void *ThreadImpl::EntryPoint(void * pthis)
{
   Thread *pt = (Thread*)pthis;
   pt->Run( pt->Argument() );

   return(&return_value);
}
#elif defined(WIN32)
unsigned __stdcall ThreadImpl::EntryPoint(void * pthis)
{
   Thread *pt = (Thread *)pthis;
   pt->Run( pt->Argument() );

   return 0;
}
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
Thread::Thread()
  : _threadimpl(NULL)
{
  _threadimpl=new ThreadImpl();
}

Thread::Thread(const Thread &model)
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"Thread::Thread(const Thread &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
Thread::~Thread()
{
	delete _threadimpl;
}

///////////////////////////////////////////////////////////////////////////////
//
Thread::ThreadId Thread::Id()
{
  return(_threadimpl->Id());
}
unsigned long Thread::RealThreadId()
{
	return(_threadimpl->RealThreadId());
}

///////////////////////////////////////////////////////////////////////////////
//
Thread::Arg Thread::Argument()
{
  return(_threadimpl->Argument());
}

///////////////////////////////////////////////////////////////////////////////
//
Thread::tState Thread::State()
{
  return _threadimpl->State();
}

///////////////////////////////////////////////////////////////////////////////
//
void Thread::Start(Arg arg)
{
  _threadimpl->Start(this,arg);
}

///////////////////////////////////////////////////////////////////////////////
//
void Thread::Join()
{
  _threadimpl->Join();
}

///////////////////////////////////////////////////////////////////////////////
//
void Thread::Resume()
{
  _threadimpl->Resume();
}

///////////////////////////////////////////////////////////////////////////////
//
void Thread::Sleep(unsigned int seconds)
#if defined(linux)
{
  struct timeval tv;
  int retval;
  
  tv.tv_sec = seconds;
  tv.tv_usec = 0;
  
  retval = select(0, NULL, NULL, NULL, &tv);
}
#elif defined(WIN32)
{
  // Since Sleep expect milliseconds, we have to convert in 
  // milli seconds (that's why we multiply the argument by 1000).
  ::Sleep(seconds*1000);  
}
#endif // void Thread::Sleep(unsigned int seconds)

///////////////////////////////////////////////////////////////////////////////
//
void Thread::SleepMs(unsigned int milliseconds)
#if defined(linux)
{
  struct timeval tv;
  int retval;
  
  tv.tv_sec = 0;
  tv.tv_usec = milliseconds*1000;
  
  retval = select(0, NULL, NULL, NULL, &tv);
}
#elif defined(WIN32)
{
  ::Sleep(milliseconds);
}
#endif // void Thread::SleepMs(unsigned int milliseconds)

///////////////////////////////////////////////////////////////////////////////
//
void Thread::Yield()
{
#if defined(linux)
  // Following function is not efficient enough!
  //  pthread_yield();
  SleepMs(1);
#elif defined(WIN32)
  // Following function is not efficient enough!
  // SwitchToThread();
  SleepMs(1);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//
Thread::ThreadId Thread::SelfId()
#if defined(linux)
{
  ThreadId theid;
  theid=pthread_self();
  return(theid);
}
#elif defined(WIN32)
{
  ThreadId theid;
  theid=GetCurrentThread();
  return(theid);
}
#endif // Thread::ThreadId Thread::SelfId()

///////////////////////////////////////////////////////////////////////////////
//
void Thread::Run(Arg arg)
{
  // The following instruction allows the pthread_create function called from 
  // the Start() function  to return and update properly the _id attributes. 
  // In some case, this attribute wasn't properly updated 
  while(State()!=RUNNING_STATE)
    {
      Yield();
    }

  PreProcess(arg);
  Execute(arg);
  PostProcess(arg);

  _threadimpl->Terminate();

#if defined(linux)
  pthread_exit(&return_value);
#elif defined(WIN32)
  ExitThread(0);
#endif

}

///////////////////////////////////////////////////////////////////////////////
//
//
void Thread::PreProcess(Arg arg)
{
  // Default intro is emtpy.
  // You may overload this function to do all your initialization stuffs.
}

///////////////////////////////////////////////////////////////////////////////
//
void Thread::PostProcess(Arg arg)
{
  // Default outtro is emtpy.
  // You may overload this function to do all your postprocessing stuffs.
}

///////////////////////////////////////////////////////////////////////////////
//
void Thread::Wait(unsigned int seconds)
{
  _threadimpl->Wait(seconds);
}

///////////////////////////////////////////////////////////////////////////////
//
void Thread::WaitMs(unsigned int milliseconds)
{
  _threadimpl->WaitMs(milliseconds);
}

///////////////////////////////////////////////////////////////////////////////
//
void Thread::Stop()
{
  _threadimpl->Stop();
}
