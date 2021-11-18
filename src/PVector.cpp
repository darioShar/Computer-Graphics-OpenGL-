#pragma once
#include "PVector.hpp"
PVector::PVector() {
	x = 0;
	y = 0;
	z = 0;
};
PVector::PVector(double x_, double y_, double z_) {
	x = x_;
	y = y_;
	z = z_;
};
void PVector::add(const PVector& v) {
	x = x + v.x;
	y = y + v.y;
	z = z + v.z;
};
PVector PVector::operator=(const PVector& other) {
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
};