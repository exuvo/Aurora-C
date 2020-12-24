// *********************************************************************************
// SmallList.hpp
// *********************************************************************************
#ifndef SMALL_LIST_HPP
#define SMALL_LIST_HPP

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <new>
#include <type_traits>

// ---------------------------------------------------------------------------------
// SmallList
// ---------------------------------------------------------------------------------
/// Stores a random-access sequence of elements similar to vector, but avoids
/// heap allocations for small lists by using an SBO. T must be trivially
/// constructible and destructible (for people using C++11 and newer, you
/// can add a static assertion using type traits to ensure this).

template <class T, int STACK_SIZE = 128>
//requires (std::is_trivially_constructible<T>::value)
class SmallList
{
public:
    // Creates an empty list.
    SmallList();

    // Creates a copy of the specified list.
    SmallList(const SmallList& other);

    // Copies the specified list.
    template <int STACK_SIZE2>
    SmallList& operator=(const SmallList<T, STACK_SIZE2>& other);

    // Destroys the list.
    ~SmallList();

    // Returns size() == 0.
    bool empty() const;

    // Returns the number of elements in the list.
    uint32_t size() const;

    // Returns the nth element.
    T& operator[](uint32_t n);

    // Returns the nth element in the list.
    const T& operator[](uint32_t n) const;

    // Returns an index to a matching element in the list or -1
    // if the element is not found.
    int find_index(const T& element) const;

    // Clears the list.
    void clear();

    // Reserves space for n elements.
    void reserve(uint32_t n);

    // Resizes the list to contain 'n' elements.
    void resize(uint32_t num, const T& fill = T());

    // Inserts an element to the back of the list.
    void push_back(const T& element);

    /// Pops an element off the back of the list.
    T pop_back();

    // Swaps the contents of this list with the other.
    void swap(SmallList& other);

    // Returns a pointer to the underlying buffer.
    T* data();

    // Returns a pointer to the underlying buffer.
    const T* data() const;
    
    class iterator {
    	public:
				iterator(T * ptr): _ptr(ptr){}
				 
				 // iterator traits
				using difference_type = size_t;
				using value_type = T;
				using pointer = const T*;
				using reference = const T&;
				using iterator_category = std::random_access_iterator_tag;
					
				//Pointer like operators
				inline const T& operator*() const { return *_ptr; }
				inline const T* operator->() const { return _ptr; }
				inline T& operator[](difference_type off) const {return _ptr[off];}
				
				//Increment / Decrement
				inline iterator& operator++() { ++_ptr; return *this; }
				inline iterator operator++(int) { return iterator(++_ptr); }
				inline iterator& operator--() { --_ptr; return *this; }
				inline iterator operator--(int) { return iterator(--_ptr); }
				
				//Arithmetic
				inline iterator& operator+=(difference_type off) {_ptr += off; return *this;}
				inline iterator& operator-=(difference_type off) {_ptr -= off; return *this;}
				friend inline iterator operator+(const iterator& x, difference_type off) {return iterator(x._ptr + off);}
				friend inline iterator operator-(const iterator& x, difference_type off) {return iterator(x._ptr - off);}
				friend inline iterator operator+(difference_type off, const iterator rhs) {rhs._ptr += off; return rhs;}
				friend inline iterator operator-(difference_type off, const iterator rhs) {rhs._ptr -= off; return rhs;}
				 
				//Comparison operators
				inline bool operator==(const iterator& rhs) const {return _ptr == rhs._ptr;}
				inline bool operator!=(const iterator& rhs) const {return _ptr != rhs._ptr;}
				inline bool operator>(const iterator& rhs) const {return _ptr > rhs._ptr;}
				inline bool operator<(const iterator& rhs) const {return _ptr < rhs._ptr;}
				inline bool operator>=(const iterator& rhs) const {return _ptr >= rhs._ptr;}
				inline bool operator<=(const iterator& rhs) const {return _ptr <= rhs._ptr;}
				
			 private:
				 T* _ptr;
		 };
    
    iterator begin() const { return iterator(ld.data); }
    iterator end() const { return iterator(ld.data + ld.num); }
    
private:
    struct ListData
    {
        ListData();
        T buf[STACK_SIZE];
        T* data;
        uint32_t num;
        uint32_t cap;
    };
    ListData ld;
};

// ---------------------------------------------------------------------------------
// SmallList
// ---------------------------------------------------------------------------------
/// Provides an indexed free list with constant-time removals from anywhere
/// in the list without invalidating indices. T must be trivially constructible
/// and destructible.
template <class T>
class FreeList
{
public:
    /// Creates a new free list.
    FreeList();

    /// Inserts an element to the free list and returns an index to it.
    int insert(const T& element);

    // Removes the nth element from the free list.
    void erase(uint32_t n);

    // Removes all elements from the free list.
    void clear();

    // Returns the range of valid indices.
    int range() const;

    // Returns the nth element.
    T& operator[](uint32_t n);

    // Returns the nth element.
    const T& operator[](uint32_t n) const;

    // Reserves space for n elements.
    void reserve(uint32_t n);

    // Swaps the contents of the two lists.
    void swap(FreeList& other);

private:
    union FreeElement
    {
        T element;
        int next;
    };
    SmallList<FreeElement> data;
    int first_free;
};

// ---------------------------------------------------------------------------------
// SmallList Implementation
// ---------------------------------------------------------------------------------
template <class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::ListData::ListData(): data(buf), num(0), cap(STACK_SIZE) {}

template <class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::SmallList() {
	static_assert(std::is_trivially_constructible<T>::value);
}

template <class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::SmallList(const SmallList& other)
{
    if (other.ld.cap == STACK_SIZE)
    {
        ld = other.ld;
        ld.data = ld.buf;
    }
    else
    {
        reserve(other.ld.num);
        for (uint32_t j=0; j < other.size(); ++j)
            ld.data[j] = other.ld.data[j];
        ld.num = other.ld.num;
        ld.cap = other.ld.cap;
    }
}

template <class T, int STACK_SIZE>
template <int STACK_SIZE2>
SmallList<T, STACK_SIZE>& SmallList<T, STACK_SIZE>::operator=(const SmallList<T, STACK_SIZE2>& other)
{
    SmallList(other).swap(*this);
    return *this;
}

template <class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::~SmallList()
{
    if (ld.data != ld.buf)
        free(ld.data);
}

template <class T, int STACK_SIZE>
bool SmallList<T, STACK_SIZE>::empty() const
{
    return ld.num == 0;
}

template <class T, int STACK_SIZE>
uint32_t SmallList<T, STACK_SIZE>::size() const
{
    return ld.num;
}

template <class T, int STACK_SIZE>
T& SmallList<T, STACK_SIZE>::operator[](uint32_t n)
{
    assert(n < ld.num);
    return ld.data[n];
}

template <class T, int STACK_SIZE>
const T& SmallList<T, STACK_SIZE>::operator[](uint32_t n) const
{
    assert(n < ld.num);
    return ld.data[n];
}

template <class T, int STACK_SIZE>
int SmallList<T, STACK_SIZE>::find_index(const T& element) const
{
    for (uint32_t j=0; j < ld.num; ++j)
    {
        if (ld.data[j] == element)
            return j;
    }
    return -1;
}

template <class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::clear()
{
    ld.num = 0;
}

template <class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::reserve(uint32_t n) {
	if (n > ld.cap) {
		T *new_mem = static_cast<T*>(malloc(n * sizeof *ld.data));
		if (!new_mem) throw std::bad_alloc();
		memcpy(new_mem, ld.data, ld.cap * sizeof *new_mem);
		if (ld.data != ld.buf) free(ld.data);
		ld.data = new_mem;
		ld.cap = n;
	}

//	enum {
//		type_size = sizeof(T)
//	};
//	if (n > ld.cap) {
//		if (ld.cap == fixed_cap) {
//			ld.data = static_cast<T*>(malloc(n * type_size));
//			memcpy(ld.data, ld.buf, sizeof(ld.buf));
//		} else ld.data = static_cast<T*>(realloc(ld.data, n * type_size));
//		ld.cap = n;
//	}
}

template <class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::resize(uint32_t num, const T& fill)
{
    if (num > ld.cap)
        reserve(num + 1);
    if (num > ld.num)
    {
        for (uint32_t j = ld.num; j < num; ++j)
            ld.data[j] = fill;
    }
    ld.num = num;
}

template <class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::push_back(const T& element)
{
    if (ld.num >= ld.cap)
        reserve(ld.cap * 2);
    ld.data[ld.num++] = element;
}

template <class T, int STACK_SIZE>
T SmallList<T, STACK_SIZE>::pop_back()
{
    return ld.data[--ld.num];
}

template <class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::swap(SmallList& other)
{
    ListData& ld1 = ld;
    ListData& ld2 = other.ld;

    const int use_fixed1 = ld1.data == ld1.buf;
    const int use_fixed2 = ld2.data == ld2.buf;

    const ListData temp = ld1;
    ld1 = ld2;
    ld2 = temp;

    if (use_fixed1)
        ld2.data = ld2.buf;
    if (use_fixed2)
        ld1.data = ld1.buf;
}

template <class T, int STACK_SIZE>
T* SmallList<T, STACK_SIZE>::data()
{
    return ld.data;
}

template <class T, int STACK_SIZE>
const T* SmallList<T, STACK_SIZE>::data() const
{
    return ld.data;
}

// ---------------------------------------------------------------------------------
// FreeList Implementation
// ---------------------------------------------------------------------------------
template <class T>
FreeList<T>::FreeList(): first_free(-1)
{
}

template <class T>
int FreeList<T>::insert(const T& element)
{
    if (first_free != -1)
    {
        const int index = first_free;
        first_free = data[first_free].next;
        data[index].element = element;
        return index;
    }
    else
    {
        FreeElement fe;
        fe.element = element;
        data.push_back(fe);
        return data.size() - 1;
    }
}

template <class T>
void FreeList<T>::erase(uint32_t n)
{
    assert(n < data.size());
    data[n].next = first_free;
    first_free = n;
}

template <class T>
void FreeList<T>::clear()
{
    data.clear();
    first_free = -1;
}

template <class T>
int FreeList<T>::range() const
{
    return data.size();
}

template <class T>
T& FreeList<T>::operator[](uint32_t n)
{
    return data[n].element;
}

template <class T>
const T& FreeList<T>::operator[](uint32_t n) const
{
    return data[n].element;
}

template <class T>
void FreeList<T>::reserve(uint32_t n)
{
    data.reserve(n);
}

template <class T>
void FreeList<T>::swap(FreeList& other)
{
    const int temp = first_free;
    data.swap(other.data);
    first_free = other.first_free;
    other.first_free = temp;
}

#endif
