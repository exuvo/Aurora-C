/*
 * Player.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_BAG_HPP_
#define SRC_BAG_HPP_

#include <vector>

template<typename T>
class Bag: std::vector<T> {
	
	public:
		typedef std::vector<T> Vector;
		typedef typename Vector::iterator iterator;
		typedef typename Vector::const_iterator const_iterator;
		using Vector::vector; // use the constructors from vector
//		Bag operator*(const Bag&) const;
//		Bag operator+(const Bag&) const;
//		virtual ~Bag();
		
		// range-checked direct access
//		T& operator[](int i) { return Vector::at(i); } 
//		const T& operator[](int i) const { return Vector::at(i); }

		void erase(int index) {
			this[index] = std::move(this[Vector::size() - 1]);
			Vector::pop_back();
		}
		
		iterator erase(const_iterator cit) {
			iterator it = Vector::begin() + (cit - Vector::cbegin());
			delete *it;
			*it = std::move(Vector::back());
			Vector::pop_back();
			
			return it;
		}
		
//		iterator erase(const_iterator first, const_iterator last) {
//			const auto beg = begin();
//			const auto cbeg = cbegin();
//			return _M_erase(beg + (first - cbeg), beg + (last - cbeg));
//		}
};

#endif /* SRC_BAG_HPP_ */
