#ifndef SMALL_LIST_HPP
#define SMALL_LIST_HPP

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <new>
#include <type_traits>
#include <stdint.h>

/// Stores a random-access sequence of elements similar to vector, but avoids
/// heap allocations for small lists by using an SBO. T must be trivially
/// constructible and destructible (for people using C++11 and newer, you
/// can add a static assertion using type traits to ensure this).
/// Alternative is boost::small_vector

// http://eel.is/c++draft/container.requirements.general

template<class T, int STACK_SIZE = 128>
//requires (std::is_trivially_constructible<T>::value)
class SmallList {
public:
	SmallList();
	SmallList(std::initializer_list<T> values);

	SmallList(const SmallList<T, STACK_SIZE>& other);
	SmallList(SmallList<T, STACK_SIZE>&& other);

	SmallList& operator=(const SmallList<T, STACK_SIZE>& other);
	SmallList& operator=(SmallList<T, STACK_SIZE>&& other);
	
	template<int STACK_SIZE2>
	SmallList& operator=(const SmallList<T, STACK_SIZE2>& other);

	template<int STACK_SIZE2>
	SmallList& operator=(SmallList<T, STACK_SIZE2>&& other);

	// Destroys the list.
	~SmallList();

	// Returns size() == 0.
	bool empty() const;

	// Returns the number of elements in the list.
	uint32_t size() const;
	
	uint32_t capacity() const;

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
	template<class... Args>
	void resize(uint32_t num, Args&&... args);

	// Inserts an element to the back of the list.
	void push_back(const T& element);
	template<class... Args>
	void push_back(Args&&... args);

	template<int STACK_SIZE2>
	void append(const SmallList<T, STACK_SIZE2>& list);

	/// Pops an element off the back of the list.
	T pop_back();

	// Swaps the contents of this list with the other.
	void swap(SmallList& other);

	// Returns a pointer to the underlying buffer.
	T* data();

	// Returns a pointer to the underlying buffer.
	const T* data() const;
	
	class const_iterator {
	public:
		const_iterator(const T* ptr): _ptr(ptr){}
		
		// iterator traits
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = const T*;
		using reference = const T&;
		using iterator_category = std::contiguous_iterator_tag;
			
		//Pointer like operators
		inline const T& operator*() const { return *_ptr; }
		inline const T* operator->() const { return _ptr; }
		inline const T& operator[](difference_type off) const {return _ptr[off];}
		
		//Increment / Decrement
		inline const_iterator& operator++() { ++_ptr; return *this; }
		inline const_iterator operator++(int) { return const_iterator(++_ptr); }
		inline const_iterator& operator--() { --_ptr; return *this; }
		inline const_iterator operator--(int) { return const_iterator(--_ptr); }
		
		//Arithmetic
		inline const_iterator& operator+=(difference_type off) {_ptr += off; return *this;}
		inline const_iterator& operator-=(difference_type off) {_ptr -= off; return *this;}
		friend inline const_iterator operator+(const const_iterator& x, const const_iterator& y) {return const_iterator(x._ptr + y._ptr);}
		friend inline difference_type operator-(const const_iterator& x, const const_iterator& y) {return x._ptr - y._ptr;}
		friend inline const_iterator operator+(const const_iterator& x, difference_type off) {return const_iterator(x._ptr + off);}
		friend inline const_iterator operator-(const const_iterator& x, difference_type off) {return const_iterator(x._ptr - off);}
		friend inline const_iterator operator+(difference_type off, const_iterator rhs) {rhs._ptr += off; return rhs;}
		friend inline const_iterator operator-(difference_type off, const_iterator rhs) {rhs._ptr -= off; return rhs;}
		
		//Comparison operators
		inline bool operator==(const const_iterator& rhs) const {return _ptr == rhs._ptr;}
		inline bool operator!=(const const_iterator& rhs) const {return _ptr != rhs._ptr;}
		inline bool operator>(const const_iterator& rhs) const {return _ptr > rhs._ptr;}
		inline bool operator<(const const_iterator& rhs) const {return _ptr < rhs._ptr;}
		inline bool operator>=(const const_iterator& rhs) const {return _ptr >= rhs._ptr;}
		inline bool operator<=(const const_iterator& rhs) const {return _ptr <= rhs._ptr;}
		
	protected:
		const T* _ptr;
	};
	
	class iterator : public const_iterator {
	public:
		iterator(T* ptr): const_iterator(ptr){}
		
		// iterator traits
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::contiguous_iterator_tag;
		
		//Pointer like operators
		inline T& operator*() const { return *const_cast<T*>(this->_ptr); }
		inline T* operator->() const { return const_cast<T*>(this->_ptr); }
		inline T& operator[](difference_type off) const {return this->_ptr[off];}
		
		//Increment / Decrement
		inline iterator& operator++() { ++this->_ptr; return *this; }
		inline iterator operator++(int) { return iterator(++this->_ptr); }
		inline iterator& operator--() { --this->_ptr; return *this; }
		inline iterator operator--(int) { return iterator(--this->_ptr); }
		
		//Arithmetic
		inline iterator& operator+=(difference_type off) {this->_ptr += off; return *this;}
		inline iterator& operator-=(difference_type off) {this->_ptr -= off; return *this;}
		friend inline iterator operator+(const iterator& x, const iterator& y) {return iterator(x._ptr + y._ptr);}
		friend inline difference_type operator-(const iterator& x, const iterator& y) {return x._ptr - y._ptr;}
		friend inline iterator operator+(const iterator& x, difference_type off) {return iterator(x._ptr + off);}
		friend inline iterator operator-(const iterator& x, difference_type off) {return iterator(x._ptr - off);}
		friend inline iterator operator+(difference_type off, iterator rhs) {rhs._ptr += off; return rhs;}
		friend inline iterator operator-(difference_type off, iterator rhs) {rhs._ptr -= off; return rhs;}
	};
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	
private:
	struct ListData {
		T buf[STACK_SIZE];
		T* data;
		uint32_t num;
		uint32_t cap;
		
		ListData();
	};
	
	ListData ld;
};

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::ListData::ListData()
: data(buf), num(0), cap(STACK_SIZE) {}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::SmallList() {}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::SmallList(std::initializer_list<T> values) {
	for (auto t : values) {
		push_back(t);
//		push_back(std::move(t));
	}
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::SmallList(const SmallList<T, STACK_SIZE>& other) {
	if (other.ld.num > STACK_SIZE) {
		reserve(other.ld.num);
	}
	
	std::copy(other.ld.data, other.ld.data + other.ld.num, ld.data);
	ld.num = other.ld.num;
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::SmallList(SmallList<T, STACK_SIZE>&& other) {
	if (other.ld.num > STACK_SIZE) {
		if (other.ld.data != other.ld.buf) {
			ld.data = other.ld.data;
			ld.cap = other.ld.cap;
			ld.num = other.ld.num;
			
			other.ld.data = other.ld.buf;
			other.ld.cap = STACK_SIZE;
			other.ld.num = 0;
			return;
			
		} else {
			reserve(other.ld.num);
		}
	}
	
	std::move(other.ld.data, other.ld.data + other.ld.num, ld.data);
	ld.num = other.ld.num;
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>& SmallList<T, STACK_SIZE>::operator=(const SmallList<T, STACK_SIZE>& other) {
	return this->operator=<STACK_SIZE>(other);
}

template<class T, int STACK_SIZE>
template<int STACK_SIZE2>
SmallList<T, STACK_SIZE>& SmallList<T, STACK_SIZE>::operator=(const SmallList<T, STACK_SIZE2>& other) {
	if constexpr (!std::is_trivially_destructible<T>::value) {
		for (size_t i = 0; i < ld.num; i++) {
			ld.data[i].~T();
		}
		ld.num = 0;
	}
	
	if (other.ld.num > STACK_SIZE) {
		reserve(other.ld.num);
		
	} else if (ld.data != ld.buf) {
		free(ld.data);
		ld.data = ld.buf;
	}
	
	std::copy(other.ld.data, other.ld.data + other.ld.num, ld.data);
	ld.num = other.ld.num;
	return *this;
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>& SmallList<T, STACK_SIZE>::operator=(SmallList<T, STACK_SIZE>&& other) {
	return this->operator=<STACK_SIZE>(other);
}

template<class T, int STACK_SIZE>
template<int STACK_SIZE2>
SmallList<T, STACK_SIZE>& SmallList<T, STACK_SIZE>::operator=(SmallList<T, STACK_SIZE2>&& other) {
	printf("=(&&)\n");
	if constexpr (!std::is_trivially_destructible<T>::value) {
		for (size_t i = 0; i < ld.num; i++) {
			ld.data[i].~T();
		}
		ld.num = 0;
	}
	
	if (other.ld.num > STACK_SIZE) {
		
		if (other.ld.data != other.ld.buf) {
			if (ld.data != ld.buf) {
				free(ld.data);
			}
			
			ld.data = other.ld.data;
			ld.cap = other.ld.cap;
			
			other.ld.data = other.ld.buf;
			other.ld.cap = STACK_SIZE2;
			other.ld.num = 0;
			
		} else {
			reserve(other.ld.num);
			std::move(other.ld.data, other.ld.data + other.ld.num, ld.data);
		}
		
	} else {
		
		if (ld.data != ld.buf) {
			free(ld.data);
			ld.data = ld.buf;
		}
		
		std::move(other.ld.data, other.ld.data + other.ld.num, ld.data);
	}
	
	ld.num = other.ld.num;
	return *this;
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::~SmallList() {
	if constexpr (!std::is_trivially_destructible<T>::value) {
		for (size_t i = 0; i < ld.num; i++) {
			ld.data[i].~T();
		}
	}
	
	if (ld.data != ld.buf) {
		free(ld.data);
	}
}

template<class T, int STACK_SIZE>
bool SmallList<T, STACK_SIZE>::empty() const {
	return ld.num == 0;
}

template<class T, int STACK_SIZE>
uint32_t SmallList<T, STACK_SIZE>::size() const {
	return ld.num;
}

template<class T, int STACK_SIZE>
uint32_t SmallList<T, STACK_SIZE>::capacity() const {
	return ld.cap;
}

template<class T, int STACK_SIZE>
T& SmallList<T, STACK_SIZE>::operator[](uint32_t n) {
	assert(n < ld.num);
	return ld.data[n];
}

template<class T, int STACK_SIZE>
const T& SmallList<T, STACK_SIZE>::operator[](uint32_t n) const {
	assert(n < ld.num);
	return ld.data[n];
}

template<class T, int STACK_SIZE>
int SmallList<T, STACK_SIZE>::find_index(const T& element) const {
	for (uint32_t j = 0; j < ld.num; ++j) {
		if (ld.data[j] == element) {
			return j;
		}
	}
	return -1;
}

template<class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::clear() {
	if constexpr (!std::is_trivially_destructible<T>::value) {
		for (size_t i = 0; i < ld.num; i++) {
			ld.data[i].~T();
		}
	}
	
	ld.num = 0;
}

template<class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::reserve(uint32_t n) {
	if (n > ld.cap) {
		
		T* new_mem;
		
		if constexpr (std::is_trivially_move_assignable<T>::value && std::is_trivially_destructible<T>::value) {
			
			if (ld.data == ld.buf) {
				
				new_mem = static_cast<T*>(malloc(n * sizeof *ld.data));
				if (!new_mem) throw std::bad_alloc();
				
				memcpy(new_mem, ld.data, ld.cap * sizeof *new_mem);
				
			} else {
				
				new_mem = static_cast<T*>(realloc(ld.data, n * sizeof *ld.data));
				if (!new_mem) throw std::bad_alloc();
			}
			
		} else {
			
			new_mem = static_cast<T*>(malloc(n * sizeof *ld.data));
			if (!new_mem) throw std::bad_alloc();
			
			std::move(ld.data, ld.data + ld.cap, new_mem);
			
			if (ld.data != ld.buf) {
				if constexpr (!std::is_trivially_destructible<T>::value) {
					for (size_t i = 0; i < ld.num; i++) {
						ld.data[i].~T();
					}
				}
				
				free(ld.data);
			}
		}
		
		ld.data = new_mem;
		ld.cap = n;
	}
}

template<class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::resize(uint32_t num, const T& fill) {
	if (num > ld.cap) reserve(num + 1);
	if (num > ld.num) {
		for (uint32_t j = ld.num; j < num; ++j) {
			ld.data[j] = fill;
		}
	}
	ld.num = num;
}

template<class T, int STACK_SIZE>
template<class... Args>
void SmallList<T, STACK_SIZE>::resize(uint32_t num, Args&&... args) {
	if (num > ld.cap) reserve(num + 1);
	if (num > ld.num) {
		for (uint32_t j = ld.num; j < num; ++j) {
			new (ld.data + j) T(std::forward<Args>(args)...);
		}
	}
	ld.num = num;
}

template<class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::push_back(const T& element) {
	if (ld.num >= ld.cap) {
		reserve(ld.cap * 2);
	}
	
	T* newElement = ld.data + ld.num++;
	
	if constexpr (!std::is_trivially_constructible<T>::value) {
		new (newElement) T();
	}
	
	*newElement = element;
}

template<class T, int STACK_SIZE>
template<class... Args>
void SmallList<T, STACK_SIZE>::push_back(Args&&... args) {
	if (ld.num >= ld.cap) {
		reserve(ld.cap * 2);
	}
	
	new (ld.data + ld.num++) T(std::forward<Args>(args)...);
}

template<class T, int STACK_SIZE>
template<int STACK_SIZE2>
void SmallList<T, STACK_SIZE>::append(const SmallList<T, STACK_SIZE2>& list) {
	for (const T& t : list) {
		push_back(t);
	}
}

template<class T, int STACK_SIZE>
T SmallList<T, STACK_SIZE>::pop_back() {
	return std::move(ld.data[--ld.num]);
}

template<class T, int STACK_SIZE>
void SmallList<T, STACK_SIZE>::swap(SmallList& other) {
	ListData& ld1 = ld;
	ListData& ld2 = other.ld;
	
	const int use_fixed1 = ld1.data == ld1.buf;
	const int use_fixed2 = ld2.data == ld2.buf;
	
	const ListData temp = ld1;
	ld1 = ld2;
	ld2 = temp;
	
	if (use_fixed1) ld2.data = ld2.buf;
	if (use_fixed2) ld1.data = ld1.buf;
}

template<class T, int STACK_SIZE>
T* SmallList<T, STACK_SIZE>::data() {
	return ld.data;
}

template<class T, int STACK_SIZE>
const T* SmallList<T, STACK_SIZE>::data() const {
	return ld.data;
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::iterator SmallList<T, STACK_SIZE>::begin() {
	return { ld.data };
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::iterator SmallList<T, STACK_SIZE>::end() {
	return { ld.data + ld.num };
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::const_iterator SmallList<T, STACK_SIZE>::begin() const {
	return cbegin();
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::const_iterator SmallList<T, STACK_SIZE>::end() const {
	return cend();
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::const_iterator SmallList<T, STACK_SIZE>::cbegin() const {
	return { ld.data };
}

template<class T, int STACK_SIZE>
SmallList<T, STACK_SIZE>::const_iterator SmallList<T, STACK_SIZE>::cend() const {
	return { ld.data + ld.num };
}

template<class T>
std::ostream& operator<<(std::ostream& os, const SmallList<T>& list) {
	bool first = true;
	for (T t : list) {
		if (!first) {
			os << ", ";
		} else {
			first = false;
		}
		
		os << t;
	}
	return os;
}

#endif
