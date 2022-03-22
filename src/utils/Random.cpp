#include <x86intrin.h>
#include <random>

#include "Random.h"

uint32_t PCG32::operator()() {
	uint64_t x = state;
	uint32_t count = (uint32_t) (x >> 59);      // 59 = 64 - 5
	
	state = x * multiplier + increment;
	x ^= x >> 18;                               // 18 = (64 - 27)/2
	return __rord((uint32_t) (x >> 27), count);	// 27 = 32 - 5
}

uint32_t PCG32_fast::operator()() {
	uint64_t x = state;
	uint32_t count = (uint32_t) (x >> 61); // 61 = 64 - 3
	
	state = x * multiplier;
	x ^= x >> 22;
	return (uint32_t) (x >> (22 + count)); // 22 = 32 - 3 - 7
}

uint8_t PCG32::next8(uint8_t min, uint8_t max) {
	std::uniform_int_distribution<uint8_t> distribution { min, max };
	return distribution(*this);
}

uint16_t PCG32::next16(uint16_t min, uint16_t max) {
	std::uniform_int_distribution<uint16_t> distribution { min, max };
	return distribution(*this);
}

uint32_t PCG32::next32(uint32_t min, uint32_t max) {
	std::uniform_int_distribution<uint32_t> distribution { min, max };
	return distribution(*this);
}

float PCG32::nextF() {
	return std::generate_canonical<float, std::numeric_limits<float>::digits, PCG32>(*this);
}

double PCG32::nextD() {
	Random32to64<PCG32> doubler { *this };
	return std::generate_canonical<double, std::numeric_limits<double>::digits, Random32to64<PCG32>>(doubler);
}

uint8_t PCG32_fast::next8(uint8_t min, uint8_t max) {
	std::uniform_int_distribution<uint8_t> distribution { min, max };
	return distribution(*this);
}

uint16_t PCG32_fast::next16(uint16_t min, uint16_t max) {
	std::uniform_int_distribution<uint16_t> distribution { min, max };
	return distribution(*this);
}

uint32_t PCG32_fast::next32(uint32_t min, uint32_t max) {
	std::uniform_int_distribution<uint32_t> distribution { min, max };
	return distribution(*this);
}

float PCG32_fast::nextF() {
	return (next32() >> 8) * 5.9604644775390625E-8f;
//	return std::generate_canonical<float, std::numeric_limits<float>::digits, PCG32_fast>(*this);
}

double PCG32_fast::nextD() {
	Random32to64<PCG32_fast> doubler { *this };
	return (doubler() >> 11) * 1.1102230246251565E-16;
//	return std::generate_canonical<double, std::numeric_limits<double>::digits, Random32to64<PCG32_fast>>(doubler);
}
