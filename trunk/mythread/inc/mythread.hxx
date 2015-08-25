#ifndef __THREAD_HXX__
#define __THREAD_HXX__

//#include "mydll.h"
#if defined(WIN32)

#if defined(MYTHREAD_EXPORTS)
/* DLL export */
#ifdef EXPORT_MYTHREAD
#undef EXPORT_MYTHREAD
#endif
#define EXPORT_MYTHREAD __declspec(dllexport)
#else
/* EXE import */
#define EXPORT_MYTHREAD __declspec(dllimport)
//#define EXPORT
#endif
#else
#define EXPORT_MYTHREAD 
#endif

#include <string>
#include <iostream>

using namespace std;

// Following includes are necessary for the pthread_t
// and HANDLE definitions.
#if defined(linux)
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


/// \brief Exception class only thrown by Thread class. Each thrown exception 
///        instance contains the description of the exception.
///
/// An Instance of this class should be used as follow:
/// \example
/// if(err==-1)  throw ThreadException("This is the error description.");
class ThreadException
{
 public:
  /// \brief Constructor.
  /// \param desc a string containing the description of the exception.
  ThreadException(const string &desc);

  /// \brief Destructor.
  ~ThreadException();

  /// \brief Return the description of current exception instance.
  const string &Description() const;

 private:
  string _msg;
};

/// \brief Operator allowing to output exception description directly into a stream.
ostream &operator<<(ostream &os, const ThreadException &e);

// The following declares the effective class that implement
// the thread class. Thread implementation is hidden to user.
class ThreadImpl;

/// \brief Thread class allowing the user to easily create and handle threads.
///
/// This implementation should work on both Linux and Windows. To create a 
/// thread just create a class from this base class and define the Execute
/// function containing the Thread activity. When linking, you must not forget
/// to add the pthread library by using the -lpthread flag.
class EXPORT_MYTHREAD Thread
{
 public:
  /// This friend relation is necessary because ThreadImpl is actually the 
  /// effective class that implement the thread.
  friend class ThreadImpl;

#if defined(linux)
  typedef pthread_t  ThreadId;
#elif defined(WIN32)
  typedef HANDLE  ThreadId;
#else
#error "Unknown Operating System"
#endif
  typedef void * Arg;

  /// tState type enumerate the possible states of a thread during his life.
  ///
  typedef   enum {
    /// \brief Encountering such state should never happen.
    UNDEFINED_STATE=0,    

    /// \brief Thread just created (constructor called). Not yet 
    /// running.
    INITIALIZED_STATE=1,  

    /// \brief Running (Start() called).
    RUNNING_STATE=2,      
    
    /// \brief Sleeping voluntary (but should still be considered 
    /// as running).
    SLEEPING_STATE=6,     

    /// \brief Stopped voluntary (actually executing an empty loop).
    /// A thread cannot stop another thread. A given thread can only
    /// stop himself by calling Stop. But resuming should be performed
    /// by another thread.
    STOPPED_STATE=512,    
    
    /// \brief Task has been entirely processed (no more running). Thread 
    /// instances in such state might be 'joined' by main thread in
    /// order to get the returned value (Linux only) by current thread instance.
    /// They should also be destroyed in order to release all used ressources.
    FINISHED_STATE=32768 
  } tState;
  
  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Constructor.
  ///
  /// A newly created thread is in INITIALIZED_STATE, meaning it is ready to 
  /// start executing a task. So creating an instance of this class doesn't 
  /// effectively fork. Forking occurs when calling the Start() function.
  ///
  /// NOTE: As no fork is done during the creation, calling the Id() and 
  /// Argument() functions just after having created the thread has no meaning
  /// and so throw an exception.
  Thread();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Destructor
  virtual ~Thread();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Returns the thread ID of current instance.
  ///
  /// WARNING: This function should be called only once the targeted thread has
  /// been started. Indeed, the ID is only assigned once started. If this function
  /// is called and the thread isn't running, then a ThreadException is thrown.
  /// To be sure a thread is running before calling this function, you can use 
  /// the State() function to check.
  ThreadId Id();

  unsigned long RealThreadId();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Returns the given argument when executed with Start.
  ///
  /// WARNING: This function should be called only once the targeted thread has
  /// been started. Indeed, the argument is only assigned once started. If this 
  /// function is called and the thread isn't running, then a ThreadException is
  /// thrown.
  Arg Argument();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Returns the current state of current thread instance.
  ///
  /// WARNING: The only state that is stable in time is FINISHED_STATE. All other 
  /// states may change depending on what the questionned and even the others 
  /// threads are doing. So this function is useful when used from only one thread
  /// that has the exclusive responsability of the questioned thread.
  ///
  /// For example, if this function returns INITIALIZED_STATE for a given thread 
  /// meaning that this thread has just been created, this doesn't mean that it 
  /// will stay so just after the call. Another thread may ask this same thread to 
  /// start executing his task. The developer must use this function with care.
  virtual tState State();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Starts or eventually restarts (because it is finished) the thread instance.
  ///
  /// That is, ask the targeted thread to begin effectively  his task into a new 
  /// separated thread.
  ///
  /// WARNING: If the targeted thread is already running, then a ThreadException 
  /// is thrown. The targeted thread must be whether in INITIALIZED_STATE or 
  /// FINISHED_STATE to make the call to this function semantically correct.
  void Start(Arg a=NULL);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Synchronize the calling thread with the termination of the targeted thread.
  ///
  /// This function blocks the calling thread until the targeted thread terminates 
  /// his task (in TERMINATED_STATE and executed pthread_exit).
  ///
  /// WARNING: A thread can't join itself. An exception is thrown in case such
  ///          case is detected.
  virtual void Join();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Function to be called on a thread in STOPPED_STATE. 
  ///
  /// Since the stopped thread decided himself to stop (the Stop function is protected), 
  /// only another thread can resume the current thread execution.
  /// 
  /// WARNING: If targeted thread isn't in STOPPED_STATE while calling this 
  /// function, then an exception is thrown.
  void Resume();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief The calling thread sleeps for the given amount of seconds and yied CPU to other thread.
  ///
  /// The calling thread can call this function if it would like to pause (or 
  /// sleep) for a determined amount of time (in seconds). 
  /// NOTE: This is a static function. No thread instance is needed to call this function.
  ///       So if a thread calls this function then its state won't be changed.
  /// CAUTION: The semantic of this function is the following: sleep AT LEAST the
  /// given amount of seconds. So, the calling thread may sleep longer than requested
  /// depending on the current CPU workload. Don't use this function for real time purpose.
  static void Sleep(unsigned int seconds);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief The calling thread sleeps for the given amount of milliseconds and yied CPU to other thread.
  ///
  /// The calling thread can call this function if it would like to pause (or 
  /// sleep) for a determined amount of time (in milli seconds).
  /// NOTE: This is a static function. No thread instance is needed to call this function.
  ///       So if a thread calls this function then its state won't be changed.
  /// CAUTION: The semantic of this function is the following: sleep AT LEAST the
  /// given amount of milliseconds. So, the calling thread may sleep longer than requested
  /// depending on the current CPU workload. Don't use this function for real time purpose.
  static void SleepMs(unsigned int milliseconds);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Yield the CPU to another thread or process.
  ///
  /// This function should be called when calling thread has nothing to do or is 
  /// waiting for an event that didn't occured yet. It is a good practice to call 
  /// this function within an infinite loop that is waiting for some asynchronous 
  /// conditions to be verified in order to reduce CPU consumption.
#if defined(Yield)
#undef Yield
#endif
  static void Yield();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Returns the current thread ID, that is the ID of the thread calling 
  /// and executing this function. No object is required to call this function.
  /// 
  static ThreadId SelfId();

 protected:
  ///////////////////////////////////////////////////////////////////////////////
  ///
  void Run(Arg arg);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Overloadable function executed just after 'Start' is called and before 'Execute' is executed.
  ///
  /// You may overload this function to do all your initialization stuffs.
  /// The given argument is the same argument than the one given to Start.
  ///
  /// NOTE: The default implementation does nothing (empty).
  virtual void PreProcess(Arg arg);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Function that must be overloaded and implemented and that contains the task of the thread.
  ///
  /// You must implement here what the thread should do. That's why is is virtual
  /// pure: to force you implementing the operations of the thread.
  /// The given argument is the same argument than the one given to Start.
  ///
  /// NOTE: You MUST NOT call pthread_exit into the implementation of this function.
  /// If you really want to do that, then just returning from this function is enough.
  virtual void Execute(Arg arg)=0;

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Overloadable function executed just after 'Execute' and before pthread_exit is called.
  /// You may overload this function to do all your postprocessing stuffs.
  /// The given argument is the same argument than the one given to 'Start'.
  ///
  /// NOTE: The default implementation does nothing (empty).
  virtual void PostProcess(Arg arg);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Change the state of running thread in Sleeping state for the given amount of time.
  /// 
  /// A running thread can call this function if it would like to pause (or 
  /// sleep) for a determined amount of time (in seconds). The execution will be suspended
  /// for the given amount of time.
  ///
  /// NOTE: Only the thread itself can call this function. So you must not use an instance
  ///       to call this function.
  ///
  /// CAUTION: The semantic of this function is the following: sleep AT LEAST the
  /// given amount of seconds. So, the calling thread may sleep longer than requested
  /// depending on the current CPU workload. Don't use this function for real time purpose.
  void Wait(unsigned int seconds);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Change the state of running thread in Sleeping state for the given amount of time.
  ///
  /// A running thread can call this function if it would like to pause (or 
  /// sleep) for a determined amount of time (in milli seconds).
  ///
  /// NOTE: Only the thread itself can call this function. So you must not use an instance
  ///       to call this function.
  ///
  /// CAUTION: The semantic of this function is the following: sleep AT LEAST the
  /// given amount of seconds. So, the calling thread may sleep longer than requested
  /// depending on the current CPU workload. Don't use this function for real time purpose.
  void WaitMs(unsigned int milliseconds);

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Change the state of running thread in Stopped state until another
  ///        thread request a resume on this same instance.
  ///
  /// A running thread can call this function if it would like to pause (or 
  /// sleep) for an UNDEFINED amount of time. So another thread should be able
  /// to resume it. While being in this state, the calling thread will stop his
  /// execution (actually, it will execute an infinite loop by yielding CPU).
  ///
  /// WARNING: Avoid calling this function while locking a mutex else you expose
  /// yourself to deadlock risk.
  void Stop();

  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Special function necessary for implementation details. This function
  ///        should never be used by programmer.
  /// This function allows the class thread to be implemented on the pthread
  /// library since the pthread_create expects a function of this signature as
  /// task to be executed by the created thread.
  static void *EntryPoint(void * pthis);

 private:
  Thread(const Thread &model);

  /// \brief Thread implementation hidden to the user.
  /// 
  /// The thread implementation is based on the pthread library on Linux
  ThreadImpl *_threadimpl; 
};

#endif //__THREAD_HXX__
