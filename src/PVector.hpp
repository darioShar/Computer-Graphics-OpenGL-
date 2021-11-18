#pragma once
#ifndef _PVector
#define _PVector
class PVector {
public:
	double x;
	double y;
	double z;
	PVector();
	PVector(double x_, double y_, double z_);
	void add(const PVector& v);
	PVector operator=(const PVector& other);
};
#endif