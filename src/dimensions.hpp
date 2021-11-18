#pragma once

struct square_shape {
	float length;
};

struct cylinder_shape {
	float radius;
	float height;
};

extern struct dimensions {
	static square_shape terrain;
	static cylinder_shape tree;
	static cylinder_shape mushroom;
};