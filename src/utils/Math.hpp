/*
 * Math.hpp
 *
 *  Created on: 23 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_MATH_HPP_
#define SRC_UTILS_MATH_HPP_

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

using Eigen::Vector2i;
using Eigen::Vector2f;
using Eigen::Vector2d;
typedef Eigen::Matrix<int64_t, 2, 1> Vector2l;

template <typename T> inline constexpr
int signum(T x, std::false_type is_signed) {
    return T(0) < x;
}

template <typename T> inline constexpr
int signum(T x, std::true_type is_signed) {
    return (T(0) < x) - (x < T(0));
}

template <typename T> inline constexpr
int signum(T x) {
    return signum(x, std::is_signed<T>());
}

Vector2l vectorsLerp(const Vector2l& a, const Vector2l& b, const uint64_t current, const uint64_t max);
int64_t vectorsCross(const Vector2l& a, const Vector2l& b);
double vectorsAngle(const Vector2l& a, const Vector2l& b);
double vectorAngle(const Vector2l& a);
double vectorLength(const Vector2l& a);
Vector2l vectorRotate(const Vector2l& a, const double angleRad);

#endif /* SRC_UTILS_MATH_HPP_ */
