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
	const long KILO = 1000;
	const long MEGA = 1000 * KILO;
	const long GIGA = 1000 * MEGA;
	const long TERA = 1000 * GIGA;
	
	const long CUBIC_DECIMETRE = 1000;
	const long CUBIC_METRE     = 1000 * CUBIC_DECIMETRE;
	
	const long NANO_MICRO = 1000;
	const long NANO_MILLI = 1000 * NANO_MICRO;
	const long NANO_SECOND = 1000 * NANO_MILLI;
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
bool rectangleContains(const Eigen::Matrix<T, 2, 2>& mat, const Eigen::Matrix<T, 2, 1>& vec) {
	T x = mat.row(0).x();
	T y = mat.row(0).y();
	T width = mat.row(1).x() - x;
	T height = mat.row(1).y() - y;
	return x <= vec.x() && x + width >= vec.x() && y <= vec.y() && y + height >= vec.y();
}

namespace Eigen {
	template <typename T>
	std::ostream& operator<<(std::ostream& os, const Eigen::Matrix<T, 2, 1>& v) {
		return os << v.x() << "," << v.y();
	}
	template <typename T>
	std::ostream& operator<<(std::ostream& os, const Eigen::Matrix<T, 2, 2>& v) {
		return os << v(0,0) << "," << v(0,1) << "," << v(1,0) << "," << v(1,1);
	}
}

int64_t vectorsCross(const Vector2l& a, const Vector2l& b);
float vectorDistanceFast(const Vector2f& a, const Vector2f& b);
double vectorDistanceFast(const Vector2d& a, const Vector2d& b);
int32_t vectorDistanceFast(const Vector2i& a, const Vector2i& b);
int64_t vectorDistanceFast(const Vector2l& a, const Vector2l& b);
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

template<typename T>
double vectorDistance(const Eigen::Matrix<T, 2, 1>& a, const Eigen::Matrix<T, 2, 1>& b) {
//	long x = b.x() - a.x();
//	long y = b.y() - a.y();
//	return std::hypot(x, y); // hypot usually takes twice as long as sqrt even with FMA
	Eigen::Matrix<double, 2, 1> diff = (b - a).template cast<double>();
	return std::sqrt(diff.dot(diff));
}

double getPositiveRootOfQuadraticEquation(double a, double b, double c);
std::optional<double> getPositiveRootOfQuadraticEquationSafe(double a, double b, double c);

double exponentialAverage(double newValue, double expAverage, double delay);

#endif /* SRC_UTILS_MATH_HPP_ */
