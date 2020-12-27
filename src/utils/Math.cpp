/*
 * Math.cpp
 *
 *  Created on: 24 Nov 2020
 *      Author: exuvo
 */

#include "Math.hpp"

Vector2l vectorsLerp(const Vector2l& a, const Vector2l& b, const uint64_t current, const uint64_t max) {
	uint64_t invAlpha = max - current;
	
	__int128_t x1 = a.x();
	__int128_t y1 = a.y();
	x1 *= invAlpha;
	y1 *= invAlpha;
	
	__int128_t x2 = b.x();
	__int128_t y2 = b.y();
	x2 *= current;
	y2 *= current;
	
	return Vector2l { (x1 + x2) / max, (y1 + y2) / max };
}

int64_t vectorsCross(const Vector2l& a, const Vector2l& b) {
//	return a.x() * b.y() - a.y() * b.x();
	__int128_t x1 = a.x();
	__int128_t y1 = a.y();
	__int128_t x2 = b.x();
	__int128_t y2 = b.y();
	return x1 * y2 - y1 * x2;
}

/*
* Errors between -5% (on axes) and +3% (on lobes) and an average error of +0.043%.
* From https://gamedev.stackexchange.com/a/69255/142645 and https://www.flipcode.com/archives/Fast_Approximate_Distance_Functions.shtml
*/
double vectorDistanceFast(const Vector2d& a, const Vector2d& b) {
	double dx = std::abs(b.x() - a.x());
	double dy = std::abs(b.y() - a.y());
	
	return 0.394 * (dx + dy) + 0.554 * std::max(dx, dy);
}

float vectorDistanceFast(const Vector2f& a, const Vector2f& b) {
	float dx = std::abs(b.x() - a.x());
	float dy = std::abs(b.y() - a.y());
	
	return 0.394f * (dx + dy) + 0.554f * std::max(dx, dy);
}

/*
* Errors between -1.5% (on axes) and +7.5% (on lobes) and an average error of ?%.
* From https://www.flipcode.com/archives/Fast_Approximate_Distance_Functions.shtml
*/
int32_t vectorDistanceFast(const Vector2i& a, const Vector2i& b) {
	int32_t max = std::abs(b.x() - a.x());
	int32_t min = std::abs(b.y() - a.y());
	
	if (min > max) {
		std::swap(max, min);
	}
	
	int32_t approx = 1007 * max + 441 * min;
	
	if (max < 16 * min) {
		approx -= 40 * max;
	}
	
	// add 512 for proper rounding
	return (approx + 512) >> 10; // div 1024
}

int64_t vectorDistanceFast(const Vector2l& a, const Vector2l& b) {
	int64_t max = std::abs(b.x() - a.x());
	int64_t min = std::abs(b.y() - a.y());
	
	if (min > max) {
		std::swap(max, min);
	}
	
	int64_t approx = 1007 * max + 441 * min;
	
	if (max < 16 * min) {
		approx -= 40 * max;
	}
	
	// add 512 for proper rounding
	return (approx + 512) >> 10; // div 1024
}

// counter clock wise rotation
Vector2l vectorRotate(const Vector2l& a, const double angleRad) {
	Eigen::Rotation2D rotation { angleRad };
	Vector2d b = static_cast<Vector2d>(rotation * a.cast<double>()); // cast only for eclipse indexer
	return b.cast<int64_t>();
}

Vector2d vectorRotate(const Vector2d& a, const double angleRad) {
	Eigen::Rotation2D rotation { angleRad };
	return static_cast<Vector2d>(rotation * a); // cast only for eclipse indexer
}

Vector2l vectorRotateDeg(const Vector2l& a, const double angleDegrees) {
	return vectorRotate(a, angleDegrees * std::numbers::pi / 180);
}

Vector2d vectorRotateDeg(const Vector2d& a, const double angleDegrees) {
	return vectorRotate(a, angleDegrees * std::numbers::pi / 180);
}

double getPositiveRootOfQuadraticEquation(double a, double b, double c) {
	return (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
}
		
std::optional<double> getPositiveRootOfQuadraticEquationSafe(double a, double b, double c) {
	double tmp = b * b - 4 * a * c;
	
	if (tmp < 0) { // square root of a negative number, no interception is possible
		return {};
	}
	
	return (-b + sqrt(tmp)) / (2 * a);
}

double exponentialAverage(double newValue, double expAverage, double delay) {
	return newValue + std::pow(std::numbers::e, -1.0 / delay) * (expAverage - newValue);
}
