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
* Errors between -1.5% (on axes) and +7.5% (on lobes) and an average error of +0.043%.
* From https://gamedev.stackexchange.com/a/69255/142645 and https://www.flipcode.com/archives/Fast_Approximate_Distance_Functions.shtml
*/
double vectorDistanceFast(const Vector2l& a, const Vector2l& b) {
	long dx = std::abs(b.x() - a.x());
	long dy = std::abs(b.y() - a.y());
	
	long min, max;
	
	if (dx < dy) {
		min = dx;
		max = dy;
	} else {
		min = dy;
		max = dx;
	}
	
	long approx = 1007 * max + 441 * min;
	
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
