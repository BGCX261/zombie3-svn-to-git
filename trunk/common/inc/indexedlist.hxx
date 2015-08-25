#ifndef _INDEXEDLIST_HXX_
#define _INDEXEDLIST_HXX_

// qlexqndre

#include "mytypes.h"
#include "newrealloc.hxx"
#include <assert.h>

template <class T> class IndexedList;

/// \brief Structure that will be used as iterator for the indexedlist.
/// This structure provides the basic iterator operations.
template<typename T>
struct _indexedlist_iterator
{
  typedef _indexedlist_iterator<T>    _Self;
  typedef IndexedList<T>             *_Node;
  typedef T                           value_type;
  typedef T*                          pointer;
  typedef T&                          reference;
  
  typedef tUInt32 tIndex;
#if defined (__GNUC__)
  static const tIndex undefined=(tIndex)-1;
#else
   static const tIndex undefined;
#endif

  _indexedlist_iterator()
    : _t(0), _index(undefined)
  { 
  }

  _indexedlist_iterator(const _indexedlist_iterator &model)
    : _t(model._t), _index(model._index)
  { 
  }

  _indexedlist_iterator(IndexedList<T> *t, tUInt32 index)
    : _t(t), _index(index)
  { 
  }
  
  ~_indexedlist_iterator()
  { 
  }

  reference operator*() const
  { 
    return (*_t)[_index];
  }
  
  pointer operator->() const
  {
    return &(*_t)[_index];
  }  
  
  _Self& operator++()
  {
    if (_index!=undefined)
      {
	_index=_t->_next_link[_index];
      }
    return *this;
  }
  
  _Self& operator=(const _indexedlist_iterator &model)
  {
    _t=model._t;
    _index=model._index;
    return *this;
  }

  _Self& operator--()
  {
    if (_index!=undefined)
      {
	_index=_t->_prev_link[_index];
      }
    return *this;
  }

  _Self operator++(int)
  {
    _Self __tmp = *this;

    if (_index!=undefined)
      {
	_index=_t->_next_link[_index];
      }

    return __tmp;
  }
  
  _Self operator--(int)
  {
    _Self __tmp = *this;

    if (_index!=undefined)
      {
	_index=_t->_prev_link[_index];
      }

    return __tmp;
  }

  tBool operator==(const _Self& __x) const
  { 
    return ((_t==__x._t) && (_index == __x._index));
  }
  
  tBool operator!=(const _Self& __x) const
  { 
    return ((_t!=__x._t) || (_index != __x._index));
  }

  tUInt32 prev() const 
  { 
    tUInt32 res;
    if (_index!=undefined)
      {
	res=_t->_prev_link[_index];
      }
    else
      {
	res=undefined;
      }
    return res;
  }

  tUInt32 next() const 
  { 
    tUInt32 res;
    if (_index!=undefined)
      {
	res=_t->_next_link[_index];
      }
    else
      {
	res=undefined;
      }
    return res;
  }

  tUInt32 index() const { return _index; }

  _Node _t;
  tUInt32 _index;
};


#if !defined(__GNUC__)
// To be able to compile with VC++, we need to declare
// this static member attribute outside the class.
template <class T>
//const _indexedlist_iterator<T>::tIndex _indexedlist_iterator<T>::undefined=-1;
const tUInt32 _indexedlist_iterator<T>::undefined=-1;
#endif


/// \brief This is a standard double linked list that has have both auto growth and index capability.
/// 
/// Actually, index capability should not be compared to array. The index
/// at which the data will be stored is chosen by one of the functions insert_front, 
/// insert_back, insert_after and insert_before. In return, these functions returns
/// the index at which the new recored has been inserted.
/// With this index, it is possible to directly access to the data by using
/// the [] operator on the list or removing it.
template <class T> class IndexedList
{
public:
  typedef tUInt32 tIndex;
#if defined(__GNUC__)
  static const tIndex undefined=(tIndex)-1;
#else
  static const tIndex undefined;
#endif
  typedef _indexedlist_iterator<T> iterator;

public:
  /// \brief Constructor. By default, 16 records space are reserved in advance.
  ///
  /// If more than that amount is inserted, then auto growth is triggered.
  IndexedList(tIndex initial_size=16)
    : _used_head(undefined), _first_free_cell(undefined), _container(0), _prev_link(0),
      _next_link(0),_usage(0), _current_array_size(initial_size),_size(0)
  {
    // Note that we instanciate one more cell as specified by 'initial_size'.
    // In fact, the last one will be used to identify the 'end of the list'
    // and won't be used to store any value.
    _container=new T[initial_size+1];
    _prev_link=new tIndex[initial_size+1];
    _next_link=new tIndex[initial_size+1];
    _usage = new tChar[initial_size+1];

    clear();
  }

  ~IndexedList()
  {
#if !defined(NOCLEAN)
    // We clean everything to 0 so that we can eventually
    // detect crazy pointers pointing on destroyed objects.
    for (tIndex i=0; i<=_current_array_size; i++)
      {
	_prev_link[i]=0;
	_next_link[i]=0;
	_usage[i]=0;
      }
#endif
    
    delete [] _container;
    delete [] _next_link;
    delete [] _prev_link;
    delete [] _usage;
    
#if !defined(NOCLEAN)
    _container=0;
    _next_link=0;
    _prev_link=0;
    _usage=0;
    _first_free_cell=0;
    _used_head=0;
    _current_array_size=0;
    _size=0;
#endif

  }
  
  /// \brief Insert the element just after the specified location.
  ///
  /// If the specified location is the end of the list, then
  /// the element is simply put at the end of the list.
  tIndex insert_after(const iterator &ii, const T &element)
  {
    tIndex prev,current, next;
    iterator i(ii);

    // Do we still have at least one remaining free cell to store 
    // this new element? If no, then just returning the undefined 
    // value to report the caller for the problem.
    if (_first_free_cell==undefined) 
      {
	// Here we are going to grow. But if we do that and if 
	// the iterator i given as parameter points on the end(),
	// then after the following function call, end() will be 
	// changed. So we might need to correct the input parameter
	// 'i' if it is equal to end() in order for our algorithm 
	// to be consistent.
	bool is_end=false;
	if (i==end()) is_end=true;

	if (!grow())
	  {
	    return undefined;
	  }

	// Here we just grew. Should we update the value of i?
	if (is_end) i=end(); 
      }
    
    // Checking whether iterator is really related to current indexed list.
    if (i._t != this) return undefined;
    if (i._index>_current_array_size) return undefined;
    
    // Here we are sure the iterator is related to the current list and
    // its index has an appropriate value.
    
    // Here we are sure we will be able to store the new inserted element.
    // So just storing it into the first free cell.
    _container[_first_free_cell]=element;
    _usage[_first_free_cell]=1;
    
    // Determining the previous, current and next indexes of the newly 
    // inserted element.
    
    if (i._index!=_current_array_size)
      {
	prev=i._index;
      }
    else
      {
	// Here we are trying to insert after the end of the list.
	// So actually, we have to insert just after the previous element 
	// before the end.
	prev=_prev_link[_current_array_size];
      }
    current=_first_free_cell;
    
    if (prev==undefined)
      {
	next=_current_array_size;
      }
    else
      {
	next=_next_link[prev];
      }
    
    // Next free index should be updated (free cells are linked).
    _first_free_cell=_next_link[_first_free_cell];
    
    // Establishing the new links between elements.
    _prev_link[current]=prev;
    _next_link[current]=next;
    
    _prev_link[next]=current;
    
    if (prev!=undefined)
      {
	_next_link[prev]=current;
      }
    else
      {
	_used_head=current;
      }

    _size++;
    return current;
    
  }

  /// \brief Insert the element at the very end of current list.
  tIndex push_back(const T &element)
  {
    return insert_before(end(),element);
  }

  /// \brief Insert the element at the very beginning of current list.
  tIndex push_front(const T &element)
  {
    return insert_before(begin(),element);
  }

  /// \brief Insert the element just before the specified location.
  ///
  tIndex insert_before(const iterator &ii, const T &element)
  {
    tIndex prev,current, next;
    iterator i(ii);
    
    // Do we still have at least one remaining free cell to store 
    // this new element? If no, then just returning the undefined 
    // value to report the caller for the problem.
    if (_first_free_cell==undefined)
      {
	// Here we are going to grow. But if we do that and if 
	// the iterator i given as parameter points on the end(),
	// then after the following function call, end() will be 
	// changed. So we might need to correct the input parameter
	// 'i' if it is equal to end() in order for our algorithm 
	// to be consistent.
	bool is_end=false;
	if (i==end()) is_end=true;

	if (!grow())
	  {
	    return undefined;
	  }

	// Here we just grew. Should we update the value of i?
	if (is_end) i=end(); 
      }
    
    // Checking whether iterator is really related to current indexed list.
    if (i._t != this) return undefined;
    if (i._index>_current_array_size) return undefined;
    
    // Here we are sure the iterator is related to the current list and
    // its index has an appropriate value.
    
    // Here we are sure we will be able to store the new inserted element.
    // So just storing it into the first free cell.
    _container[_first_free_cell]=element;
    _usage[_first_free_cell]=1;
    
    // Determining the previous, current and next indexes of the newly 
    // inserted element.
    
    next=i._index;
    current=_first_free_cell;

    // 'next' is sure to exist because we insert BEFORE something 
    // already existing (with used it as reference). 
    // But if the list is empty, then prev might be undefined.
    prev=_prev_link[next];
    
    // Next free index should be updated (free cells are linked).
    _first_free_cell=_next_link[_first_free_cell];
    
    // Establishing the new links between elements.
    _prev_link[current]=prev;
    _next_link[current]=next;

    // Following is always true even if next points on the end of the list.
    _prev_link[next]=current;
    
    if (prev!=undefined)
      {
	_next_link[prev]=current;
      }
    else
      {
	_used_head=current;
      }
    
    _size++;
    return current;
  
  }

  /// \brief Remove a given record from its index.
  tBool remove(tIndex index)
  {
    if (index>=_current_array_size) return false;

    // Is the target index really associated to a used slot?
    if (_usage[index]==0) return false;

    // Here we are sure the slot at that index is really used.
    // Just releasing its usage.
    _usage[index]=0;

    if (_prev_link[index]!=undefined)
      {
	_next_link[_prev_link[index]]=_next_link[index];
    }
    else
      {
	_used_head=_next_link[index];
      }
    
    _prev_link[_next_link[index]]=_prev_link[index];
    
    _next_link[index]=_first_free_cell;
    
    if (_first_free_cell!=undefined)
      {
	_prev_link[_first_free_cell]=index;
      }
    
    _prev_link[index]=undefined;
    
    _first_free_cell=index;
    _size--;

    return true;
  }
  
  /// \brief Remove a given record pointed by the given iterator.
  ///
  /// CAUTION: After this call, the given iterator has no meaning anymore
  /// and no other operation should be performed on that iterator.
  void remove(iterator &i)
  {
    if (i._t != this) return;
    remove(i._index);
  }

  /// \brief Empty the list it is applied to.
  void clear()
  {
    // Creating the links. At creation, all the cells are free. We link
    // all the free cells together so that we can find them and so use 
    // them later.
    for (tUInt32 i=0; i<=_current_array_size; i++)
      {
	_prev_link[i]=(tIndex) i-1;
	_next_link[i]=(tIndex) i+1;
	_usage[i]=0;
      }
    
    // The first free cell is the head of all linked free cells. 
    _first_free_cell=0;
      
      // Last link must point to a undefined value so that we can recognize it.
      _prev_link[0]=undefined;
    _next_link[_current_array_size-1]=undefined;
    
    // End of the list must not have any predecessor or successor at
    // the very beginning.
    _prev_link[_current_array_size]=undefined;
    _next_link[_current_array_size]=undefined;
    // The last slot is used by the 'end' record.
    _usage[_current_array_size]=1;
    _container[_current_array_size]=T();
    _used_head=_current_array_size;

    _size=0;
  }

  /// \brief Returns an iterator on the very beginning of the list.
  iterator begin()
  {
    iterator it(this, _used_head);
    return it;
  }

  /// \brief Returns an iterator at the very end of the list.
  iterator end() 
  {
    iterator it(this, _current_array_size);
    return it;
  }

  /// \brief Returns the number of inserted element.
  inline tIndex size() const { return _size; }

  /// \brief Returns the number of records that can be inserted without triggering list growth.
  inline tIndex free_size() const { return _current_array_size-_size; }

  /// \brief Direct access to a given record.
  inline T &operator [](tIndex i) 
  { 
    return _container[i]; 
  }

  /// \brief Returns whether or not a given index exists.
  inline tBool exists(tIndex i) 
  { 
    if (i>=_current_array_size) 
      {
	return false; 
      }
    return _usage[i]==1;
  }
  
protected:

  /// \brief Reallocate current 
  bool grow()
  {
    tIndex new_array_size=_current_array_size*2;

    _prev_link=new_realloc(_prev_link,  _current_array_size+1, new_array_size+1);

    if (!_prev_link) return false;

    _next_link=new_realloc(_next_link, _current_array_size+1, new_array_size+1);

    if (!_next_link) return false;

    _usage=new_realloc(_usage, _current_array_size+1, new_array_size+1);
    if (!_usage) return false;

    _container=new_realloc(_container, _current_array_size+1, new_array_size+1);
    if (!_container) return false;
    
    for (tUInt32 i=_current_array_size+1; i<=new_array_size; i++)
      {
	_prev_link[i]=(tIndex) i-1;
	_next_link[i]=(tIndex) i+1;
	_usage[i]=0;
      }
    
    _next_link[new_array_size-1]=undefined;
    _prev_link[new_array_size]=_prev_link[_current_array_size];
    _next_link[new_array_size]=undefined;
    _container[new_array_size]=T();
    _usage[new_array_size]=1;

    // The last inserted record should now be linked with the new 
    // end of list.
    if (_prev_link[_current_array_size]!=undefined)
      {
	_next_link[_prev_link[_current_array_size]]=new_array_size;
      }
    
    // Updating the first free cell if necessary.
    if (_first_free_cell==undefined)
      {
	_prev_link[_current_array_size]=undefined;
	_first_free_cell=_current_array_size;
      }
    else
      {
	_prev_link[_current_array_size]=_first_free_cell;
	_next_link[_first_free_cell]=_current_array_size;
      }

    // Now the former end record becomes a free record
    // that need to be appended to the rest of newly
    // created free records.
    _next_link[_current_array_size]=_current_array_size+1;
    _usage[_current_array_size]=0;
      
    // Update the new indexedlist capacity
    _current_array_size=new_array_size;

    return true;
  }

  friend struct _indexedlist_iterator<T>;
  tIndex _used_head;
  tIndex _first_free_cell;
  T *_container;
  tIndex *_prev_link;
  tIndex *_next_link;
  tChar *_usage;
  tIndex _current_array_size;
  tIndex _size;

private:
  // Copy constructor is private to FORCE the user to instantiate 
  // a new object with 'new' instead of static allocation.
  // This is because we cannot copy this object so easily 
  // mainly because of internal objects.
  IndexedList(const IndexedList &model) { assert(0); }
};


#if !defined(__GNUC__)
// To be able to compile with VC++, we need to declare
// this static member attribute outside the class.
template <class T>
//const IndexedList<T>::tIndex IndexedList<T>::undefined=-1;
const tUInt32 IndexedList<T>::undefined=-1;
#endif



#endif //_INDEXEDLIST_HXX_
