#include "mytimer.hxx"
#include "mythread.hxx"
#include "mymutex.hxx"
#include "indexedlist.hxx"

#if defined(__GNUC__)
#include <sys/time.h>
#else
#include <winbase.h>
#endif

#define UNDEF_TIME ((tAbsoluteTime)-1)

#include <iostream>
#include <mytypes.h>
#include <assert.h>

using namespace std;

class ScheduledEvent
{
public:
  typedef tUInt64 tAbsoluteTime;
  ScheduledEvent();
  ScheduledEvent(const ScheduledEvent &model);
  ScheduledEvent(tAbsoluteTime at, bool *p_flag);
  ~ScheduledEvent();

  tAbsoluteTime Time() const { return _absolute_time; }
  tBool *Flag() const { return _p_flag; }
  void Alarm() { *_p_flag=true; }
  void Clear() { *_p_flag=false; }

  bool operator <(ScheduledEvent &e);
  bool operator >(ScheduledEvent &e);
  bool operator <=(ScheduledEvent &e);
  bool operator >=(ScheduledEvent &e);
  bool operator ==(ScheduledEvent &e);
  bool operator !=(ScheduledEvent &e);

protected:
  tAbsoluteTime _absolute_time;
  tBool *_p_flag;

private:

};

#if defined(__GNUC__)
ostream &operator <<(ostream &s, const ScheduledEvent &e)
{
  s<<"[Alarm="<<e.Time()<<" Flag("<<e.Flag()<<")="<<*e.Flag()<<"]";
  return s;
}
#endif

ScheduledEvent::ScheduledEvent()
  : _absolute_time(0), _p_flag(NULL)
{
}

ScheduledEvent::ScheduledEvent(tAbsoluteTime at, bool *p_flag)
  : _absolute_time(at), _p_flag(p_flag)
{
}

ScheduledEvent::ScheduledEvent(const ScheduledEvent &model)
  : _absolute_time(model._absolute_time), _p_flag(model._p_flag)
{
}

ScheduledEvent::~ScheduledEvent()
{
  _absolute_time=0;
  _p_flag=0;
}

bool ScheduledEvent::operator <(ScheduledEvent &e)
{
  return ( (tAbsoluteTime) _absolute_time< (tAbsoluteTime) e._absolute_time);
}

bool ScheduledEvent::operator <=(ScheduledEvent &e)
{
  return ( (tAbsoluteTime) _absolute_time <= (tAbsoluteTime) e._absolute_time);
}

bool ScheduledEvent::operator >(ScheduledEvent &e)
{
  return ( (tAbsoluteTime) _absolute_time > (tAbsoluteTime) e._absolute_time);
}

bool ScheduledEvent::operator >=(ScheduledEvent &e)
{
  return ( (tAbsoluteTime) _absolute_time >= (tAbsoluteTime) e._absolute_time);
}

bool ScheduledEvent::operator ==(ScheduledEvent &e)
{
  return ( (tAbsoluteTime) _absolute_time == (tAbsoluteTime) e._absolute_time);
}

bool ScheduledEvent::operator !=(ScheduledEvent &e)
{
  return ( (tAbsoluteTime) _absolute_time != (tAbsoluteTime) e._absolute_time);
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////
class Scheduler : public Thread
{
public:
  typedef IndexedList<ScheduledEvent>::tIndex tIndex;
  typedef Timer::tTimeInterval tTimeInterval;
  typedef Timer::tFlag tFlag;
  typedef Timer::tTimerId tTimerId;
  typedef tUInt64 tAbsoluteTime;
  
  Scheduler();
  ~Scheduler();

  tIndex SetAlarmIn(tTimeInterval milliseconds, tFlag *p_flag);  
  tBool Cancel(tTimerId timerid);
  void Init();
  tAbsoluteTime GetTimeFlow();
  bool IsAlarm();
  void Shutdown();

protected:
  void Execute(Thread::Arg arg);

  void SetAlarm(tAbsoluteTime target);
  void ClearAlarm();
  tAbsoluteTime GetAlarm();
#if defined(__GNUC__)
  void SetTimeFlow(struct timeval *tp);
#else
  void SetTimeFlow(DWORD tp);
#endif
  void RingAlarm();  
  Mutex _absolutetime_protect;
  Mutex _alarm_protect;
  Mutex _list_protect;
  bool _shutdown;
  tAbsoluteTime _absolutetime;
  tAbsoluteTime _alarmtime;
  IndexedList<ScheduledEvent> _agenda;
#if !defined(__GNUC__)
  DWORD _overflow_counter;
  DWORD _history_tp;
#endif
private:
  Scheduler(const Scheduler &model);
};

Scheduler::Scheduler()
  : Thread(), _absolutetime_protect(), _alarm_protect(),
    _list_protect(), _shutdown(false), _absolutetime(0), 
    _alarmtime(UNDEF_TIME)
#if !defined(__GNUC__)
	,_overflow_counter(),_history_tp()
#endif
{
  // At least initializing the absolute time with current date.
  Init();
}

Scheduler::Scheduler(const Scheduler &model)
  : Thread(), _absolutetime_protect(), _alarm_protect(),
    _list_protect(), _shutdown(false), _absolutetime(0), 
    _alarmtime(UNDEF_TIME)
{
#if !defined(RELEASE)
  // This constructor should never be used!
  cerr<<"Scheduler::Scheduler(const Scheduler &model): This function is not supposed to be called! Crashing."<<endl; cerr.flush();
  assert(0);
#endif
}

Scheduler::~Scheduler()
{
  _shutdown=true;
}


bool Scheduler::IsAlarm()
{ 
  bool res;
  if (GetAlarm()==UNDEF_TIME) 
    {
      res=false; 
    }
  else if (GetAlarm()<GetTimeFlow())
    {
      res=true; 
    }
  else 
    {
      res=false; 
    }
  return res;
}

void Scheduler::SetAlarm(Scheduler::tAbsoluteTime target)
{
  _alarm_protect.Lock();
  _alarmtime=(tAbsoluteTime) target;
#if defined(DEBUG)
  //  cout<<"Scheduler::SetAlarm: "<<target<<endl;
#endif

  _alarm_protect.Unlock();
}

void Scheduler::ClearAlarm()
{
  _alarm_protect.Lock();
  _alarmtime=UNDEF_TIME;
  _alarm_protect.Unlock();
}

Scheduler::tAbsoluteTime Scheduler::GetAlarm()
{
  tAbsoluteTime res;
  _alarm_protect.Lock();
  res=_alarmtime;
  _alarm_protect.Unlock();
  return res;
}

Scheduler::tAbsoluteTime Scheduler::GetTimeFlow()
{
  tAbsoluteTime res;
  _absolutetime_protect.Lock();
  res=_absolutetime;
  _absolutetime_protect.Unlock();
  return res;
}

#if defined(__GNUC__)
void Scheduler::SetTimeFlow(struct timeval *tp)
{
  tAbsoluteTime res;

  res=((tAbsoluteTime) tp->tv_sec*1000000)+tp->tv_usec;

  _absolutetime_protect.Lock();
  _absolutetime=res;
  _absolutetime_protect.Unlock();
}
#else
void Scheduler::SetTimeFlow(DWORD tp)
{
  tAbsoluteTime res;

  res=(tAbsoluteTime) tp*1000 + 0xFFFFFFFFL*_overflow_counter;

  _absolutetime_protect.Lock();
  _absolutetime=res;
  _absolutetime_protect.Unlock();
}
#endif

void Scheduler::Init()
{

#if defined(__GNUC__)
  struct timeval tp;
  gettimeofday(&tp,NULL);
  SetTimeFlow(&tp);
#else
   DWORD tp;
   tp=GetTickCount();
   if (tp<_history_tp)
   {
	   _overflow_counter++;
   }
   _history_tp=tp;
   SetTimeFlow(tp);
#endif
  ClearAlarm(); // No alarm at the beginning.
}

void Scheduler::Execute(Thread::Arg arg)
{
  while(!_shutdown)
    {
#if defined(__GNUC__)
      struct timeval tp;
      gettimeofday(&tp,NULL);
      SetTimeFlow(&tp);
#else
      DWORD tp;
      tp=GetTickCount();
      if (tp<_history_tp)
	  {
	     _overflow_counter++;
	  }
      _history_tp=tp;
      SetTimeFlow(tp);
#endif
      Thread::Yield();

      if (!IsAlarm())
	{
	  continue;
	}

      RingAlarm();

    } // while(!_shutdown)
  
}

void Scheduler::RingAlarm()
{
  IndexedList<ScheduledEvent>::iterator it;
  _list_protect.Lock();
  it=_agenda.begin();

#if defined(DEBUG)
  //      cout<<"Scheduler::RingAlarm: "<<endl;
#endif

  // For each scheduled event that are at the beginning of the agenda
  // and which scheduled has passed the current time, we have to set
  // the alarm flag.
  while(it!=_agenda.end())
    {
      if ( it->Time()>GetTimeFlow()) 
	{
	  // Here we found one scheduled event that is in the future.
	  // So this means that all the other events are also in the
	  // future (since they are time ordered at creation time)
	  // so we don't need to go further. Just stopping here.
	  break;
	}

#if defined(DEBUG)
      //      cout<<"Scheduler::RingAlarm: Timer #"<<it.index()<<" alarmed."<<endl;
#endif

      // Here we found one scheduled event that has just passed.
      // We have to set the alarm for this event.
      (*it).Alarm();

      // Since the event has passed and we set the alarm, we can
      // safely remove it from the agenda.
      _agenda.remove(it);

      // Since we just removed the head, just requesting 
      // the new head of the agenda (because the previous iterator
      // should have been corrupted due to the previous remove operation).
      it=_agenda.begin();
    } // while(it<>_agenda.end())
  
  // Now looking whether we expect some alarm.
  if (it == _agenda.end())
    {
      // Here the agenda is empty! Just clearing the alarm.
      ClearAlarm();
    }
  else
    {
      // Here the agenda is not empty. Setting the alarm
      // for the head of the list.
      SetAlarm(_agenda.begin()->Time());
    }
  _list_protect.Unlock();
}

tBool Scheduler::Cancel(Timer::tTimerId timerid)
{
  tBool res;
  IndexedList<ScheduledEvent>::iterator it;
  _list_protect.Lock();
  if (_agenda.exists(timerid))
    {
      _agenda[timerid].Clear();
    }
  res=_agenda.remove(timerid);
  it=_agenda.begin();
  _list_protect.Unlock();

#if defined(DEBUG)
  //  cout<<"Scheduler::Cancel: Timer #"<<timerid<<endl;
#endif
  if (it==_agenda.end())
    {
      // No more event are scheduled. Just clearing the
      // alarm flag.
      ClearAlarm();
      return res;
    }

  SetAlarm((*it).Time());

  return res;
}



IndexedList<ScheduledEvent>::tIndex Scheduler::SetAlarmIn(Timer::tTimeInterval milliseconds, Timer::tFlag *p_flag)
{
  Timer::tTimerId res;

  tAbsoluteTime schedule;

  schedule=GetTimeFlow();

  // Since we keep our aboslute time in micro seconds, we have
  // to multiply by 1000.
  schedule+=milliseconds*1000;

  ScheduledEvent e(schedule, p_flag);
  IndexedList<ScheduledEvent>::iterator it;

#if defined(DEBUG)
  //  cout<<"Scheduler::SetAlarmIn: "<<schedule<<endl;
#endif

  // Looking now where to insert this new event.
  _list_protect.Lock();
  it=_agenda.begin();

  // We will insert the new event in order to get a 
  // ordered list in increasing order.
  while((it!=_agenda.end()) && (e>(*it)) )
    {
      it++;
    }

  // Here whether we reached the end of the list of we reached 
  // a record that is later than the current one.
  // Whatever the reason we break the loop, it contains the 
  // position where to insert before!
  res=_agenda.insert_before(it,e);
  _list_protect.Unlock();  

  // In case there is no space anymore to store this event
  // we have to report it to the caller by returning the
  // undefined value. Note that the flag won't be forced to
  // be false.
  if (res!=IndexedList<ScheduledEvent>::undefined)
    {
      (*p_flag)=false;

      // Setup the next absolute alarm date.
      if (GetAlarm()==UNDEF_TIME)
	{
	  // If alarm not already set, just setting it to 
	  // the newly created event.
	  SetAlarm(schedule);
	}
      else
	{
	  // If already set, then updating only if 
	  // currently new added alarm is sooner as
	  // the currently registered one.
	  if (schedule<GetAlarm())
	    {
	      SetAlarm(schedule);
	    }
	}
    }

  //  dump(_agenda);

  return res;
}

void Scheduler::Shutdown() 
{ 
  _shutdown = true; 
}

static Scheduler *_scheduler;

// Only one client may use this timer object at a given time.
// This is necessary in order to avoid race condition about getting
// an already used timerid. Prior to version v0.9.4, such race 
// condition existed leading to some bug (like the SUDH server 
// reaching the maximum number of allowed running thread when only
// 2 connections at any given time were running).
// 
// So, following mutex is used in order to avoid multiple parallel
// usage of the timer object. This also allows us to avoid multiple
// initializations of the same timer from different threads.
static Mutex _timer_access_protect;

Timer::tAbsoluteTime Timer::GetAbsoluteTime()
{
  Timer::tAbsoluteTime res;

  // The first caller will start the Scheduler thread.
  _timer_access_protect.Lock();

  if (!_scheduler)
    {
      _scheduler=new Scheduler();
      // Following function (Init) is very important for the current
      // function because it will set the current time so that the 
      // GetTimeFlow can return a good value.
      //
      // NOTE: We don't need to start the thread here.
      _scheduler->Init();
      _scheduler->Start();
    }

  res=_scheduler->GetTimeFlow();

  _timer_access_protect.Unlock();

  return res;
}

Timer::tTimerId Timer::Create(Timer::tTimeInterval milliseconds, Timer::tFlag *p_flag)
{
  Timer::tTimerId res;

  // Only one thread should access the timer object!
  _timer_access_protect.Lock();

  // The first caller will start the Scheduler thread.
  if (!_scheduler)
    {
      _scheduler=new Scheduler();
      // Following function (Init) is very important for the current
      // function because it will set the current time so that the 
      // GetTimeFlow can return a good value.
      //
      // NOTE: We don't need to start the thread here.
      _scheduler->Init();
      _scheduler->Start();
    }

  res=_scheduler->SetAlarmIn(milliseconds, p_flag);

  _timer_access_protect.Unlock();

  return res;
}

tBool Timer::Cancel(Timer::tTimerId timerid)
{
  tBool res;

  _timer_access_protect.Lock();
  res=_scheduler->Cancel(timerid);
  _timer_access_protect.Unlock();

  return res;
}

void Timer::Shutdown()
{
  tUInt32 count;
  _timer_access_protect.Lock();
  if (_scheduler)
    {
      _scheduler->Shutdown();
      count=0;
      while( (_scheduler->State()!=Thread::FINISHED_STATE) && (count<50))
	{ 
	  count++;
	  Thread::SleepMs(100); 
	}

      if (count<50)
	{
	  _scheduler->Join();
	}
      delete _scheduler;
      _scheduler=0;
    } 
  _timer_access_protect.Unlock();
}
