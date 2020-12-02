/*
 * Math.hpp
 *
 *  Created on: 23 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_MATH_HPP_
#define SRC_UTILS_MATH_HPP_

#include <numbers>
#include <optional>

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

namespace Units {
		const double AU = 149597870.7; // In km
		const double C = 299792.458; // In km
};

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

constexpr double toDegrees(double radians) {
	return radians * 57.29577951308232;
}

constexpr double toRadians(double degrees) {
	return degrees * 0.017453292519943295;
}

__attribute__((warn_unused_result)) Vector2l vectorsLerp(const Vector2l& a, const Vector2l& b, const uint64_t current, const uint64_t max);
__attribute__((warn_unused_result)) int64_t vectorsCross(const Vector2l& a, const Vector2l& b);
__attribute__((warn_unused_result)) double vectorsAngle(const Vector2l& a, const Vector2l& b);
__attribute__((warn_unused_result)) double vectorAngle(const Vector2l& a);
__attribute__((warn_unused_result)) double vectorLength(const Vector2l& a);
__attribute__((warn_unused_result)) Vector2l vectorRotate(const Vector2l& a, const double angleRad);
__attribute__((warn_unused_result)) Vector2d vectorRotate(const Vector2d& a, const double angleRad);

__attribute__((warn_unused_result)) double getPositiveRootOfQuadraticEquation(double a, double b, double c);
__attribute__((warn_unused_result)) std::optional<double> getPositiveRootOfQuadraticEquationSafe(double a, double b, double c);

#endif /* SRC_UTILS_MATH_HPP_ */
