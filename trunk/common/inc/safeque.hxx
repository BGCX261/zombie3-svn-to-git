#ifndef __SAFEQUE_HXX__
#define __SAFEQUE_HXX__

#include <deque>
#include "mymutex.hxx"

template <typename T>
class SafeQue
{
public:
////////////////////TYPEDEFS////////////////////////////////////////////////////////
  typedef deque<T> tQueue;

////////////////////Public Methods//////////////////////////////////////////////////
  SafeQue();
  ~SafeQue();
  void Push(const T elem);
  T Pop();
  void Clear();
  tUInt32 Size() const;
  bool Empty() const;

private:
  tQueue _queue;
  Mutex _mutex;
};

template <typename T>
SafeQue<T>::SafeQue()
  : _queue(), _mutex()
{
}

template <typename T>
SafeQue<T>::~SafeQue()
{
}

template <typename T>
void SafeQue<T>::Push(const T elem)
{
  _mutex.Lock();
  _queue.push_back(elem);
  _mutex.Unlock();
}

template <typename T>
T SafeQue<T>::Pop()
{
  T ret=NULL;
  
  _mutex.Lock();
  
  if (!_queue.empty())
  {
  	ret = _queue.front();
  	
	_queue.pop_front();
  }

  _mutex.Unlock();
  
  return ret;
}

template <typename T>
void SafeQue<T>::Clear()
{
	_mutex.Lock();
	_queue.clear();
	_mutex.Unlock();
}

template <typename T>
tUInt32 SafeQue<T>::Size() const
{
  //Not critical, so no need to synchronize
  return static_cast<tUInt32>(_queue.size());
}

template <typename T>
bool SafeQue<T>::Empty() const
{
  //Not critical, so no need to synchronize
  return _queue.empty();
}

#endif	//__SAFEQUE_HXX__
