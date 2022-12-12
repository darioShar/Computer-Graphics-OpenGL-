#include "utils.hpp"
#include <random>
#include <chrono>


std::vector<vcl::vec2> get_random_points(int total_points, float radius_collision) {
	srand(time(0));
	std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
	std::vector<vcl::vec2> points;
	// We consider the plane to be a rectangular division with elementary rectangles
	// of size radius_collision
	if (radius_collision != 0.0f) {
		std::vector<std::vector<bool> > map;
		int num_obj_axis = (int)(1.0f / radius_collision);
		map.resize(num_obj_axis);
		for (auto& m : map) m.resize(num_obj_axis, false);
		std::uniform_int_distribution<int> distrib(0, num_obj_axis - 1);
		// Attention total capacity
		total_points = std::min(total_points, num_obj_axis);
		while (total_points) {
			vcl::vec2 p(distrib(gen), distrib(gen));
			if (map[p[0]][p[1]]) continue;
			else map[p[0]][p[1]] = true;
			p *= radius_collision;
			points.push_back(p);
			total_points--;
		}
	}
	else {
		std::uniform_real_distribution<float> distrib(0, 1.0f);
		while (total_points--)
			points.push_back(vcl::vec2(distrib(gen), distrib(gen)));
	}
	return points;
}

vcl::vec3 cross_product(const vcl::vec3& v_A, const vcl::vec3& v_B) {
	vcl::vec3 c_P;
	c_P[0] = v_A[1] * v_B[2] - v_A[2] * v_B[1];
	c_P[1] = -(v_A[0] * v_B[2] - v_A[2] * v_B[0]);
	c_P[2] = v_A[0] * v_B[1] - v_A[1] * v_B[0];
	return c_P;
}

// a b
// c d
float det(float a, float b, float c, float d) {
	return a * d - b * c;
}

vcl::mat3 inverse_matrix(vcl::mat3 a) {
	// formule de la comatrice
	vcl::mat3 inv_matrix;
	float determinant = a[0][0] * ((a[1][1] * a[2][2]) - (a[2][1] * a[1][2])) - a[0][1] * (a[1][0]
		* a[2][2] - a[2][0] * a[1][2]) + a[0][2] * (a[1][0] * a[2][1] - a[2][0] * a[1][1]);
	
	assert_vcl(std::abs(determinant) > 1e-5, "Error : trying to invert matrix of null determinant");

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			inv_matrix[i][j] = det(	a[(i + 1) % 3][(j + 1) % 3], a[(i + 1) % 3][(j + 2) % 3],
									a[(i + 2) % 3][(j + 1) % 3], a[(i + 2) % 3][(j + 2) % 3]);
		}
	}

	inv_matrix /= determinant;
	return inv_matrix;
}