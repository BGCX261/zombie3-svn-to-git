#ifndef _NEWREALLOC_HXX_
#define _NEWREALLOC_HXX_

#include <algorithm>
#include <iostream>
#include <cstddef>

using namespace std;

template <typename T>
T *new_realloc(T *array, size_t old_size, size_t new_size)
{
   T *temp = new T[new_size];

   if (! temp) return NULL;

   copy(array, array + old_size, temp);

   delete [] array;
   array=NULL;

   return temp;
}

#if 0

// Code provided by Dave Kondrad in the FAQ/Message Thread of:
// http://www.codeproject.com/tips/newandmalloc.asp 
// 
// Eventually adapted by Raffi SEMERCIYAN

// This new_realloc function is needed because 
// malloc cannot be simply used in C++ context.

///////////////////////////////////////////////////////////////////////////////
// Copy functors
///////////////////////////////////////////////////////////////////////////////

template<class T>
struct assign_copy
{
  void operator() (T &dest, const T &src)
  {
    dest = src;
  }
};

///////////////////////////////////////////////////////////////////////////////

template<class T>
struct member_copy
{
  void operator() (T &dest, const T &src)
  {
    dest.copy(src);
  }
};

///////////////////////////////////////////////////////////////////////////////
// Realloc Using New
///////////////////////////////////////////////////////////////////////////////

template <class T, class C>
T *new_realloc(T *ptr, 
               unsigned long oldSize,
               unsigned long newSize,
               const C &copyop)
{
  unsigned long size;
  T *newPtr;

  if(newSize > 0)
  {
    newPtr  = new T [newSize];

    if(newSize > oldSize)
      size = oldSize;
    else
      size = newSize;

    for(unsigned long index = 0; index < size; ++index)
    {
      copyop(newPtr[index], ptr[index]);
    }
  }

  if(oldSize > 1)
    delete [] ptr;
  else
    delete ptr;

  return newPtr;
}

#endif

#endif // _NEWREALLOC_HXX_
