/*
 * BitVector.hpp
 *
 *  Created on: Dec 27, 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_BITVECTOR_HPP_
#define SRC_UTILS_BITVECTOR_HPP_

#include <stdint.h>
#include <vector>

class BitVector {
	public:
		BitVector() = default;
		BitVector(const BitVector&) = default;
		BitVector(BitVector&&) = default;
		
		void reserve(uint32_t capacity);
		uint32_t size();
		void clear();
		
		class _bitReference {
			public:
				_bitReference(uint64_t* ptr, uint64_t mask): ptr(ptr), mask(mask) {};
				_bitReference(const _bitReference&) = default;
				operator bool() const;
				_bitReference& operator =(bool value);
				_bitReference& operator =(const _bitReference& value);
				bool operator ==(const _bitReference& value) const;
			private:
				uint64_t* ptr;
				uint64_t mask;
		};
		
		bool operator [](const uint32_t index) const;
		_bitReference operator [](const uint32_t index);
		uint64_t& operator ()(const uint32_t index);
		
		void operator =(const BitVector& bv);
		bool operator ==(const BitVector& bv);
		
		void operator &=(const BitVector& bv);
		void operator |=(const BitVector& bv);
		void operator ^=(const BitVector& bv);
		// And not
		void operator %=(const BitVector& bv);
		
		// Returns true if the specified BitVector has any bits set to true that are also set to true in this BitVector.
		bool intersects(const BitVector& bv);
		
		// Returns true if this bit set is a super set of the specified set,
		// i.e. it has all bits set to true that are also set to true in the specified BitVector.
		bool containsAll(const BitVector& bv);
		
		uint32_t cardinality();
		
		class iterator {
			public:
				iterator(BitVector& bv, uint32_t idx): bv(bv), longIdx(idx >> 6), bitIdx(idx), val(bv.data[longIdx] >> (idx % 64)) {}
				
				// iterator traits
				using difference_type = uint32_t;
				using value_type = _bitReference;
				using iterator_category = std::forward_iterator_tag;
					
				//Pointer like operators
				inline uint32_t operator*() const { return bitIdx; }
				inline uint32_t operator->() const { return bitIdx; }
//				inline _bitReference operator[](difference_type off) const {return bv[bitIdx + off];}
				
				//Increment / Decrement
				iterator& operator++() {
					val >>= 1;
					bitIdx++;
					
					if (val == 0) {
						while (val == 0 && longIdx < bv.data.size() - 1) {
							val = bv.data[++longIdx];
						}
						
						bitIdx = longIdx * 64;
						
						if (val == 0) {
							bitIdx += 63; // end
						} else {
							bitIdx += __builtin_ffsl(val) - 1;
						}
					}
					
					return *this; 
				}
//				inline iterator operator++(int) { return iterator(bv, ++idx); }
//				inline iterator& operator--() { --idx; return *this; }
//				inline iterator operator--(int) { return iterator(bv, --idx); }
				
				//Arithmetic
//				inline iterator& operator+=(difference_type off) {idx += off; return *this;}
//				inline iterator& operator-=(difference_type off) {idx -= off; return *this;}
//				friend inline iterator operator+(const iterator& x, difference_type off) {return iterator(x.bv, x.idx + off);}
//				friend inline iterator operator-(const iterator& x, difference_type off) {return iterator(x.bv, x.idx - off);}
//				friend inline iterator operator+(difference_type off, iterator rhs) {rhs.idx += off; return rhs;}
//				friend inline iterator operator-(difference_type off, iterator rhs) {rhs.idx -= off; return rhs;}
				 
				//Comparison operators
				inline bool operator==(const iterator& rhs) const {return bitIdx == rhs.bitIdx;}
				inline bool operator!=(const iterator& rhs) const {return bitIdx != rhs.bitIdx;}
				inline bool operator>(const iterator& rhs) const {return bitIdx > rhs.bitIdx;}
				inline bool operator<(const iterator& rhs) const {return bitIdx < rhs.bitIdx;}
				inline bool operator>=(const iterator& rhs) const {return bitIdx >= rhs.bitIdx;}
				inline bool operator<=(const iterator& rhs) const {return bitIdx <= rhs.bitIdx;}
				
				private:
					BitVector& bv;
					uint32_t longIdx;
					uint32_t bitIdx;
					uint64_t val;
		 };
		
		iterator begin() {
			uint32_t longIdx = 0;
			
			while (data[longIdx] == 0 && longIdx < data.size() - 1) {
				longIdx++;
			}
			
			uint64_t val = data[longIdx];
			
			if (val == 0) {
				return end();
			}
			
			uint32_t bitIdx = longIdx * 64;
			bitIdx += __builtin_ffsl(val) - 1;
			
			return iterator(*this, bitIdx);
		}
		iterator end() { return iterator(*this, size() - 1); }
		
	private:
		std::vector<uint64_t> data;
		static constexpr uint64_t LONG_MASK = 0xFFFFFFFFFFFFFFFF;
		static constexpr uint64_t BV(uint32_t index) {
			return 1 << (index & LONG_MASK);
		};
		
};



#endif /* SRC_UTILS_BITVECTOR_HPP_ */
