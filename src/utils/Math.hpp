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

using Eigen::Matrix2i;
using Eigen::Matrix2f;
using Eigen::Matrix2d;
typedef Eigen::Matrix<int64_t, 2, 2> Matrix2l;

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

template <typename T>
bool rectagleContains(const Eigen::Matrix<T, 2, 2>& mat, const Eigen::Matrix<T, 2, 1>& vec) {
	T x = mat.col(0).x();
	T y = mat.col(0).y();
	T width = mat.col(1).x() - x;
	T height = mat.col(1).y() - y;
	return x <= vec.x() && x + width >= vec.x() && y <= vec.y() && y + height >= vec.y();
}

namespace Eigen {
	std::ostream& operator<<(std::ostream& os, const Vector2i& v);
	std::ostream& operator<<(std::ostream& os, const Vector2l& v);
	std::ostream& operator<<(std::ostream& os, const Vector2f& v);
	std::ostream& operator<<(std::ostream& os, const Vector2d& v);
}

int64_t vectorsCross(const Vector2l& a, const Vector2l& b);
double vectorDistanceFast(const Vector2l& a, const Vector2l& b);
__attribute__((warn_unused_result)) Vector2l vectorsLerp(const Vector2l& a, const Vector2l& b, const uint64_t current, const uint64_t max);
__attribute__((warn_unused_result)) Vector2l vectorRotate(const Vector2l& a, const double angleRad);
__attribute__((warn_unused_result)) Vector2d vectorRotate(const Vector2d& a, const double angleRad);
__attribute__((warn_unused_result)) Vector2l vectorRotateDeg(const Vector2l& a, const double angleDegrees);
__attribute__((warn_unused_result)) Vector2d vectorRotateDeg(const Vector2d& a, const double angleDegrees);

// Returns the angle in radians of this vector (point) relative to the given vector. Angles are towards the positive y-axis. (typically counter-clockwise)
// https://stackoverflow.com/questions/21483999/using-atan2-to-find-angle-between-two-vectors
template<typename T, int N>
double vectorsAngle(const Eigen::Matrix<T, 2, N>& a, const Eigen::Matrix<T, 2, N>& b) {
	return std::atan2(vectorsCross(a, b), a.dot(b));
}

template<typename T, int N>
double vectorAngle(const Eigen::Matrix<T, 2, N>& a) {
	return std::atan2(a.y(), a.x());
}

template<typename T, int N>
double vectorLength(const Eigen::Matrix<T, 2, N>& a) {
//	return std::hypot(a.x(), a.y());
	return a.norm();
}

template<typename T, int N>
double vectorDistance(const Eigen::Matrix<T, 2, N>& a, const Eigen::Matrix<T, 2, N>& b) {
//	long x = b.x() - a.x();
//	long y = b.y() - a.y();
//	return std::hypot(x, y); // hypot is usually twice as slow as sqrt even with FMA
	
	return std::sqrt(a.dot(b));
}

double getPositiveRootOfQuadraticEquation(double a, double b, double c);
std::optional<double> getPositiveRootOfQuadraticEquationSafe(double a, double b, double c);

#endif /* SRC_UTILS_MATH_HPP_ */
