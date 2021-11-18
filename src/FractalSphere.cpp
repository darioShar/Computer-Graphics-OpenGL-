#include "FractalSphere.hpp"


void FractalSphere::iterate(vcl::vec3 center_pos, float radius, vcl::vec3 local_displacement, CollisionStructure* actualStructure, int n) {
	// First set collision primitive
	actualStructure->primitive = new CollisionBall(local_displacement, radius);
	//actualStructure->create(vcl::mesh_primitive_sphere(r / (float)std::pow(2 * std::sqrt(2.0), total_iterations - n) + 0.001f));
	actualStructure->create(vcl::mesh_primitive_sphere(radius * (1.01f), vcl::vec3({ 0,0,0 }), 5, 5));

	if (n == 0) {
		final_centers.push_back(center_pos);
	}
	else {
		n--;
		CollisionStructure* left = new CollisionStructure;
		CollisionStructure* right = new CollisionStructure;
		CollisionStructure* up = new CollisionStructure;
		CollisionStructure* down = new CollisionStructure;
		CollisionStructure* front = new CollisionStructure;
		CollisionStructure* back = new CollisionStructure;
		
		vcl::vec3 displacement;
		float new_radius = radius / (2.0f * std::sqrt(2));
		displacement = vcl::vec3({ -radius / 2.0f, 0,0 });
		iterate(center_pos + displacement, new_radius, displacement, left, n);

		displacement = vcl::vec3({ radius / 2.0f, 0,0 });
		iterate(center_pos + displacement, new_radius, displacement, right, n);

		displacement = vcl::vec3({ 0.0, radius / 2.0f,0 });
		iterate(center_pos + displacement, new_radius, displacement, front, n);

		displacement = vcl::vec3({ 0.0, -radius / 2.0f,0 });
		iterate(center_pos + displacement, new_radius, displacement, back, n);

		displacement = vcl::vec3({ 0.0, 0, radius / 2.0f });
		iterate(center_pos + displacement, new_radius, displacement, up, n);

		displacement = vcl::vec3({ 0.0, 0, -radius / 2.0f });
		iterate(center_pos + displacement, new_radius, displacement, down, n);

		actualStructure->subStructures.push_back(left);
		actualStructure->subStructures.push_back(right);
		actualStructure->subStructures.push_back(up);
		actualStructure->subStructures.push_back(down);
		actualStructure->subStructures.push_back(front);
		actualStructure->subStructures.push_back(back);
	}
}


void FractalSphere::create(float _radius, vcl::vec3 pos, int iterations) {
	r = _radius;
	total_iterations = iterations;
	// final radius will be radius / 4^iterations
	final_radius = r / (float)std::pow(2*std::sqrt(2.0), iterations);

	// iterating
	structure = new CollisionStructure;
	iterate(pos, r, pos, structure, iterations);

	// updating mesh
	for (auto& center : final_centers) {
		d_mesh.push_back(vcl::mesh_primitive_sphere(final_radius, center, 20,20));
	}
	d_mesh_drawable = vcl::mesh_drawable(d_mesh);
}