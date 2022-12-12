#pragma once

#include "vcl/vcl.hpp"

/* Returns total_points points uniformly distributed in [0, 1]^2. No collision under radius given*/
std::vector<vcl::vec2> get_random_points(int total_points, float radius_collision = 0.0f);
vcl::vec3 cross_product(const vcl::vec3& v_A, const vcl::vec3& v_B);

// Must be inversible, otherwise division by zero !
vcl::mat3 inverse_matrix(vcl::mat3 mat);