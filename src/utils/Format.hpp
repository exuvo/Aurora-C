/*
 * Utils.hpp
 *
 *  Created on: 14 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_FORMAT_HPP_
#define SRC_UTILS_FORMAT_HPP_

#include <fmt/format.h>
#include <chrono>

#include "Math.hpp"

using namespace std::chrono;

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
	if ( !v.empty() ) {
		out << '[';
		for (auto it = v.cbegin(); it != v.cend(); it++) {
			if (it != v.cbegin()) {
				out << ", ";
			}
			const auto& e = *it;
			out << e;
		}
		out << "]";
	}
	return out;
}

// https://fmt.dev/latest/api.html#udt

// [m,u,r]
// m - milliseconds
// u - microseconds
// r - raw number
template<> struct fmt::formatter<nanoseconds> {
	char presentation = 'm';

	// Parses format specifications from {format options}
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();
		
		if (it != end && (*it == 'm' || *it == 'u' || *it == 'r')) {
			presentation = *it++;
		}
//		
		if (it != end && *it != '}') {
			throw format_error("invalid format");
		}
		
		return it;
	}
	
	template<typename FormatContext>
	auto format(const nanoseconds& t, FormatContext& ctx) -> decltype(ctx.out()) {
		using Units::NANO_MICRO;
		using Units::NANO_MILLI;
		
		uint64_t time = t.count();
		
		if (presentation == 'm') {
			uint32_t nanos = time % NANO_MILLI;
			uint32_t milli = time / NANO_MILLI;
			return format_to(ctx.out(), "{}.{:06}ms", milli, nanos);
			
		} else if (presentation == 'u') {
			uint32_t nanos = time % NANO_MICRO;
			uint32_t micro = time / NANO_MICRO;
			return format_to(ctx.out(), "{}.{:03}us", micro, nanos);
			
		} else { // r
			return format_to(ctx.out(), "{}", time);
		}
	}
};

#endif /* SRC_UTILS_FORMAT_HPP_ */
