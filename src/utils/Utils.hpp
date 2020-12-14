/*
 * Utils.hpp
 *
 *  Created on: 14 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_UTILS_HPP_
#define SRC_UTILS_UTILS_HPP_

#include <thread>
#include <algorithm>

#if defined(__clang__) || defined(__GNUG__)
#include <cxxabi.h>
#endif

#include "entt/entt.hpp"

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

enum class ThreadPriority {
		LOW,
		NORMAL,
		HIGH,
		HIGHER
};

void setThreadPriority(std::thread& thread, ThreadPriority prio);

template<typename T>
std::string type_name() {
	std::string tname = typeid(T).name();
#if defined(__clang__) || defined(__GNUG__)
	int status;
	char* demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
	if (status == 0) {
		tname = demangled_name;
		std::free(demangled_name);
	}
#endif
	return tname;
}

namespace entt {
	std::ostream& operator<<(std::ostream& os, const entt::entity& e);
}

template<typename T>
void vectorEraseUnorderedIdx(std::vector<T>& vec, int index) {
	assert(index < vec.size());
	vec[index] = std::move(vec[vec.size() - 1]);
	vec.pop_back();
}

template<typename T>
void vectorEraseUnorderedIter(std::vector<T>& vec, typename std::vector<T>::iterator it) {
	assert(it < vec.end());
	*it = std::move(*(vec.end() - 1));
	vec.pop_back();
}

template<typename T>
bool vectorEraseUnorderedVal(std::vector<T>& vec, const T& value) {
	typename std::vector<T>::iterator position = std::find(vec.begin(), vec.end(), value);
	if (position != vec.end()) {
    vectorEraseUnorderedIter(vec, position);
    return true;
	}
	return false;
}

#endif /* SRC_UTILS_UTILS_HPP_ */
