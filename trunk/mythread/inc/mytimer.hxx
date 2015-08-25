#ifndef _TIMER_HXX_
#define _TIMER_HXX_

#include "mytypes.h"
#include "mydll.h"

namespace Timer
{
  typedef tUInt32 tTimeInterval;  // milli seconds!
  typedef tBool   tFlag;
  typedef tUInt32 tTimerId;
#if !defined(__GNUC__)
  // For Visual C++, we have to use Signed 64 bits integer
  // because the compler doesn't implement automatic conversion
  // from Unsigned 64 bits integer to double.
  typedef tSInt64 tAbsoluteTime;  // micro seconds!
#else
  typedef tUInt64 tAbsoluteTime;  // micro seconds!
#endif

  const tUInt32 Undefined=(tUInt32) -1;
  //static const tUInt32 Undefined;

  // Returns the absolute time in micro seconds!
  // CAUTION: 64 bits integer is returned! 
  tAbsoluteTime GetAbsoluteTime();
  tTimerId Create(tTimeInterval milliseconds, tFlag *p_flag);
  bool Cancel(tTimerId timerid);
  void Shutdown();
  
}

#define CancelTimer(timerid)  if (timerid!=Timer::Undefined) \
                              {                              \
 		                 Timer::Cancel(timerid);     \
                                 timerid=Timer::Undefined;   \
                              }

#endif // _TIMER_HXX_
