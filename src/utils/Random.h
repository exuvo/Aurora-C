#ifndef SRC_UTILS_RANDOM_H_
#define SRC_UTILS_RANDOM_H_

#include <stdint.h>

// PCG-XSH-RR https://en.wikipedia.org/wiki/Permuted_congruential_generator
// Requires 39 state bits to pass BigCrush
class PCG32 {
public:
	typedef uint32_t result_type;
	
	PCG32() {};
	PCG32(uint64_t seed) {
		state = 2 * seed + 1;
		(*this)();
	}
	
	uint32_t operator()();
	static inline constexpr uint32_t min() { return 0; };
	static inline constexpr uint32_t max() { return UINT32_MAX; };
	
	inline uint8_t next8() { return (*this)(); };
	inline uint8_t next8(uint8_t max) { return next8(0, max); };
	uint8_t next8(uint8_t min, uint8_t max);
	inline uint16_t next16() { return (*this)(); };
	inline uint16_t next16(uint16_t max) { return next16(0, max); };
	uint16_t next16(uint16_t min, uint16_t max);
	inline uint32_t next32() { return (*this)(); };
	inline uint32_t next32(uint32_t max) { return next32(0, max); };
	uint32_t next32(uint32_t min, uint32_t max);
	float nextF();
	double nextD();

private:
	uint64_t state = 0xcafef00dd15ea5e5; // Or something seed-dependent
	static inline constexpr const uint64_t multiplier = 6364136223846793005;
	static inline constexpr const uint64_t increment = 1442695040888963407; // Or an arbitrary odd constant
};

// PCG-XSH-RS Period 2^62
// Requires 49 state bits to pass BigCrush
class PCG32_fast {
public:
	typedef uint32_t result_type;
	
	PCG32_fast() {};
	PCG32_fast(uint64_t seed) {
		state = 2 * seed + 1;
		(*this)();
	}
	
	uint32_t operator()();
	static inline constexpr uint32_t min() { return 0; };
	static inline constexpr uint32_t max() { return UINT32_MAX; };
	
	inline uint8_t next8() { return (*this)(); };
	inline uint8_t next8(uint8_t max) { return next8(0, max); };
	uint8_t next8(uint8_t min, uint8_t max);
	inline uint16_t next16() { return (*this)(); };
	inline uint16_t next16(uint16_t max) { return next16(0, max); };
	uint16_t next16(uint16_t min, uint16_t max);
	inline uint32_t next32() { return (*this)(); };
	inline uint32_t next32(uint32_t max) { return next32(0, max); };
	uint32_t next32(uint32_t min, uint32_t max);
	float nextF();
	double nextD();

private:
	uint64_t state = 0xcafef00dd15ea5e5; // Must be odd
	static inline constexpr const uint64_t multiplier = 6364136223846793005;
};

template<class a32bitRandomGenerator>
class Random32to64 {
public:
	Random32to64(a32bitRandomGenerator& gen): gen(gen) {};
	
	typedef uint64_t result_type;
	uint64_t operator()() { return (uint64_t) gen() << 32 | gen(); };
	static inline constexpr uint64_t min() { return 0; };
	static inline constexpr uint64_t max() { return UINT64_MAX; };
private:
	a32bitRandomGenerator& gen;
};

#endif /* SRC_UTILS_RANDOM_H_ */
