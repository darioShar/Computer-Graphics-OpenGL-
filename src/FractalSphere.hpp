#pragma once
#include "DefaultMeshDrawable.hpp"
#include "Collision.hpp"

class FractalSphere : public DefaultMeshDrawable {
	float r;
	float final_radius;
	int total_iterations;
	vcl::vec3 center;

	std::vector<vcl::vec3> final_centers;
	void iterate(vcl::vec3 center_pos, float radius, vcl::vec3 local_displacement, CollisionStructure* actualStructure, int n);

public :
	CollisionStructure* structure;

	void create(float radius, vcl::vec3 pos, int iterations);

};

namespace vcl
{
	template <typename SCENE>
	void draw(FractalSphere& drawable, SCENE const& scene)
	{
		draw(static_cast<DefaultMeshDrawable>(drawable), scene);
		draw(drawable.structure, scene);
	}
}