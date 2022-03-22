/*
 * BitVector.cpp
 *
 *  Created on: Dec 27, 2020
 *      Author: exuvo
 */
#include <cassert>

#include "BitVector.hpp"

BitVector::_bitReference::operator bool() const {
	return !!(*ptr & mask);
}

BitVector::_bitReference& BitVector::_bitReference::operator=(bool value) {
	if (value) {
		*ptr |= mask;
	} else {
		*ptr &= ~mask;
	}
	return *this;
}

BitVector::_bitReference& BitVector::_bitReference::operator=(const _bitReference& value) {
	return *this = bool(value);
}

bool BitVector::_bitReference::operator==(const _bitReference& value) const {
	return bool(*this) == bool(value);
}

void BitVector::reserve(uint32_t capacity) {
	size_t oldCapacity = data.capacity();
	
	if (capacity > oldCapacity) {
		uint32_t longs = (63 + capacity) / 64;
		data.reserve(longs);
		data.resize(longs);
	}
}

uint32_t BitVector::size() {
	return data.capacity() * 64;
}

void BitVector::clear() {
	for (size_t i=0; i < data.capacity(); i++) {
		data[i] = 0;
	}
}

bool BitVector::operator [](const uint32_t index) const {
	assert(index >> 6 < data.capacity());
	return data[index >> 6] & BV(index);
}

BitVector::_bitReference BitVector::operator [](const uint32_t index) {
	assert(index >> 6 < data.capacity());
	return _bitReference(&data[index >> 6], BV(index));
}

uint64_t& BitVector::operator ()(const uint32_t index) {
	assert(index < data.capacity());
	return data[index];
}

void BitVector::operator =(const BitVector& bv) {
	data = bv.data;
}

bool BitVector::operator ==(const BitVector& bv) {
	if (bv.data.capacity() != data.capacity()) {
		return false;
	}
	
	size_t size = data.capacity();
	for (size_t i=0; i < size; i++) {
		if (data[i] != bv.data[i]) {
			return false;
		}
	}
	
	return true;
}

void BitVector::operator &=(const BitVector& bv) {
	assert(data.capacity() == bv.data.capacity());
	
	size_t size = data.capacity();
	for (size_t i=0; i < size; i++) {
		data[i] &= bv.data[i];
	}
}

void BitVector::operator |=(const BitVector& bv) {
	assert(data.capacity() >= bv.data.capacity());
	
	size_t size = bv.data.capacity();
	for (size_t i=0; i < size; i++) {
		data[i] |= bv.data[i];
	}
}

void BitVector::operator ^=(const BitVector& bv) {
	assert(data.capacity() >= bv.data.capacity());
	
	size_t size = bv.data.capacity();
	for (size_t i=0; i < size; i++) {
		data[i] ^= bv.data[i];
	}
}

void BitVector::operator %=(const BitVector& bv) {
	assert(data.capacity() >= bv.data.capacity());
	
	size_t size = bv.data.capacity();
	for (size_t i=0; i < size; i++) {
		data[i] &= ~bv.data[i];
	}
}

bool BitVector::intersects(const BitVector& bv) {
	size_t size = data.capacity();
	for (size_t i=0; i < size; i++) {
		if (data[i] & bv.data[i]) {
			return true;
		}
	}
	return false;
}

bool BitVector::containsAll(const BitVector& bv) {
	size_t size = data.capacity();
	for (size_t i=0; i < size; i++) {
		if (data[i] != (data[i] & bv.data[i])) {
			return false;
		}
	}
	return true;
}

// processor specific better implementations can be found at https://github.com/WojciechMula/sse-popcount
// for less than 2000 bits __builtin_popcountll looks to be pretty good for most CPUs
uint32_t BitVector::cardinality() {
	uint32_t setBits = 0;
	
	size_t size = data.capacity();
	for (size_t i=0; i < size; i++) {
		setBits += __builtin_popcountll(data[i]);
	}
	
	return setBits;
}


BitVector32::_bitReference::operator bool() const {
	return !!(*ptr & mask);
}

BitVector32::_bitReference& BitVector32::_bitReference::operator=(bool value) {
	if (value) {
		*ptr |= mask;
	} else {
		*ptr &= ~mask;
	}
	return *this;
}

BitVector32::_bitReference& BitVector32::_bitReference::operator=(const _bitReference& value) {
	return *this = bool(value);
}

bool BitVector32::_bitReference::operator==(const _bitReference& value) const {
	return bool(*this) == bool(value);
}

void BitVector32::clear() {
	data = 0;
}

bool BitVector32::operator [](const uint8_t index) const {
	return data & BV(index);
}

BitVector32::_bitReference BitVector32::operator [](const uint8_t index) {
	return _bitReference(&data, BV(index));
}

uint32_t& BitVector32::operator ()() {
	return data;
}

uint32_t BitVector32::cardinality() {
	return __builtin_popcount(data);
}


BitVector64::_bitReference::operator bool() const {
	return !!(*ptr & mask);
}

BitVector64::_bitReference& BitVector64::_bitReference::operator=(bool value) {
	if (value) {
		*ptr |= mask;
	} else {
		*ptr &= ~mask;
	}
	return *this;
}

BitVector64::_bitReference& BitVector64::_bitReference::operator=(const _bitReference& value) {
	return *this = bool(value);
}

bool BitVector64::_bitReference::operator==(const _bitReference& value) const {
	return bool(*this) == bool(value);
}

void BitVector64::clear() {
	data = 0;
}

bool BitVector64::operator [](const uint8_t index) const {
	return data & BV(index);
}

BitVector64::_bitReference BitVector64::operator [](const uint8_t index) {
	return _bitReference(&data, BV(index));
}

uint64_t& BitVector64::operator ()() {
	return data;
}

uint32_t BitVector64::cardinality() {
	return __builtin_popcountll(data);
}
