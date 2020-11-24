/*
 * Math.cpp
 *
 *  Created on: 24 Nov 2020
 *      Author: exuvo
 */

#include <math.h>

#include "Math.hpp"

Vector2l vectorsLerp(const Vector2l a, const Vector2l b, uint64_t current, uint64_t max) {
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

// Returns the angle in radians of this vector (point) relative to the given vector. Angles are towards the positive y-axis. (typically counter-clockwise)
// https://stackoverflow.com/questions/21483999/using-atan2-to-find-angle-between-two-vectors
double vectorsAngle(const Vector2l& a, const Vector2l& b) {
	return atan2(vectorsCross(a, b), a.dot(b));
}

double vectorLength(const Vector2l& a) {
//	return hypot(a.x(), a.y());
	return a.norm();
}

// counter clock wise rotation
Vector2l vectorRotate(const Vector2l& a, const double angleRad) {
	Eigen::Rotation2D rotation { angleRad };
	Vector2d b = static_cast<Vector2d>(rotation * a.cast<double>()); // cast only for eclipse indexer
	return b.cast<int64_t>();
}

