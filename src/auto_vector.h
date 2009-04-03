#if !defined AUTO_VECTOR_H
#define AUTO_VECTOR_H
//------------------------------------
// Reliable Software (c) 2003
// www.relisoft.com
// Any use, commercial or noncommercial of this code
// is hereby granted, under the condition
// that this copyright notice be not removed.
//------------------------------------

#include <memory>
#include <vector>
#include <cassert>
#include <algorithm>

//---------------------------------
// Dynamic array of owned pointers. 
// Ownership transfer semantics.
//---------------------------------

template <class T> 
class auto_vector
{
public:
	class auto_lvalue
	{
	public:
		auto_lvalue (T * & p) : _p (p) {}
		operator T * () const { return _p; }
		T * operator-> () const { return _p; }
		auto_lvalue & operator= (std::auto_ptr<T> ap)
		{
			delete _p;
			_p = ap.release ();
			return *this;
		}
	private:
		T * & _p;
	};
public:

    explicit auto_vector (size_t capacity = 0);
    ~auto_vector ();

	bool empty() const {return _arr.empty (); }

	// memory management
    size_t  size () const { return _arr.size (); }
	size_t  capacity () const { return _arr.capacity (); }
    void reserve (size_t count);
	void resize (unsigned int newSize);
	void erase (size_t idx);
    void clear ();
	void compact ();
	void swap (auto_vector<T> & other)
	{
		_arr.swap (other._arr);
	}

	// iterators
	typedef typename std::vector<T*>::iterator				iterator;
	typedef typename std::vector<T*>::const_iterator		const_iterator;
	typedef typename std::vector<T*>::reverse_iterator		reverse_iterator;
	typedef typename std::vector<T*>::const_reverse_iterator const_reverse_iterator;

	// array access
    T const * operator [] (size_t i) const { return _arr [i]; }
	auto_lvalue operator [] (size_t i) 
	{ 
		return auto_lvalue (_arr [i]); 
	}
	void assign (size_t i, std::auto_ptr<T> p);
	void assign_direct (size_t i, T * p);
	void insert (size_t idx, std::auto_ptr<T> p);
	iterator insert (iterator pos, std::auto_ptr<T> p);
	// stack access
	void push_back (std::auto_ptr<T> p);
	std::auto_ptr<T> pop_back (); // non-standard
	T * back () { return _arr.back (); }
    T const * back () const { return _arr.back (); }
	T * front () { return _arr.front (); }
    T const * front () const { return _arr.front (); }

	iterator				begin () { return _arr.begin (); }
	iterator				end () { return _arr.end (); }
	const_iterator			begin () const { return _arr.begin (); }
	const_iterator			end () const { return _arr.end (); }
	reverse_iterator		rbegin() { return _arr.rbegin (); }
	reverse_iterator		rend()   { return _arr.rend (); }
	const_reverse_iterator	rbegin() const  { return _arr.rbegin (); }
	const_reverse_iterator	rend() const    { return _arr.rend (); }

	iterator erase (iterator it);
	iterator erase (iterator first, iterator last);

	// iterator/index conversion
	size_t ToIndex (iterator const & it);
	size_t ToIndex (reverse_iterator const & rit); 
	iterator ToIter (size_t idx);
	reverse_iterator ToRIter (size_t idx);
private:
	auto_vector (auto_vector<T> const & src);
	auto_vector<T> & operator= (auto_vector<T> const & src);
private:
	std::vector<T*>		_arr;
};

template <class T>
auto_vector<T>::auto_vector (size_t capacity)
{
	_arr.reserve (capacity);
}

template <class T>
auto_vector<T>::~auto_vector ()
{
	clear ();
}

template <class T>
void auto_vector<T>::push_back (std::auto_ptr<T> ptr)
{
    _arr.push_back (ptr.get ());
    ptr.release ();
}

template <class T>
inline std::auto_ptr<T> auto_vector<T>::pop_back () 
{
	assert (size () != 0);
	T * p = _arr.back ();
	_arr.pop_back ();
	return std::auto_ptr<T> (p);
}

template <class T>
class DeletePtr
{
public:
	void operator () (T * p)
	{
		delete p;
	}
};

template <class T>
void auto_vector<T>::clear ()
{
	std::for_each (begin (), end (), DeletePtr<T> ());
	_arr.clear ();
}

template <class T>
inline void auto_vector<T>::assign_direct (size_t i, T * p)
{
    assert (i < size ());
	if (_arr [i] != p)
		delete _arr [i];
	_arr [i] = p;
}

template <class T>
inline void auto_vector<T>::assign (size_t i, std::auto_ptr<T> p)
{
    assert (i < size ());
	if (_arr [i] != p.get ())
		delete _arr [i];
	_arr [i] = p.release ();
}

template <class T>
void auto_vector<T>::erase (size_t idx)
{
    assert (idx < size ());
	// Delete item
	delete _arr [idx];
	// Compact array
	_arr.erase (ToIter (idx));
}

template <class T>
typename auto_vector<T>::iterator auto_vector<T>::erase (typename auto_vector<T>::iterator it)
{
    assert (it < end ());
	delete *it;
	return _arr.erase (it);
}

template <class T>
typename auto_vector<T>::iterator auto_vector<T>::erase (typename auto_vector<T>::iterator first, typename auto_vector<T>::iterator last)
{
    assert (first < end ());
	assert (first < last);
	assert (last <= end ());
	std::for_each (first, last, DeletePtr<T> ());
	return _arr.erase (first, last);
}

template <class T>
void auto_vector<T>::compact ()
{
	// move null pointers to the end
	T * null = 0;
	iterator pos = std::remove (begin (), end (), null);
	_arr.resize (pos - begin ());
}

template <class T>
size_t auto_vector<T>::ToIndex (iterator const & it)  
{
	assert (it - begin () >= 0);
	return static_cast<size_t> (it - begin ());
}

template <class T>
size_t auto_vector<T>::ToIndex (reverse_iterator const & rit)  
{
	iterator it = rit.base ();
	--it;
	assert (it - begin () >= 0);
	return static_cast<size_t> (it - begin ());
}

template <class T>
typename auto_vector<T>::iterator auto_vector<T>::ToIter (size_t idx) 
{ 
	return begin () + idx;
}

template <class T>
typename auto_vector<T>::reverse_iterator auto_vector<T>::ToRIter (size_t idx) 
{
	++idx;
	return reverse_iterator (ToIter (idx));
}


template <class T>
inline void auto_vector <T>::reserve (size_t count)
{
	_arr.reserve (count);
}

template <class T>
inline void auto_vector<T>::resize (unsigned int newSize)
{
	if (newSize < size ())
		std::for_each (ToIter (newSize), end (), DeletePtr<T> ());
	_arr.resize (newSize);
}

template <class T>
void auto_vector<T>::insert (size_t idx, std::auto_ptr<T> p)
{
	assert (idx <= size ());
	_arr.insert (ToIter (idx), p.get ());
	p.release ();
}

template <class T>
typename auto_vector<T>::iterator auto_vector<T>::insert (iterator pos, std::auto_ptr<T> p)
{
	assert (pos <= end ());
	iterator newpos = _arr.insert (pos, p.get ());
	p.release ();
	return newpos;
}

#endif
