#pragma once
#include <vcl/vcl.hpp>
#include "DescriptiveAnimation.hpp"
#include "Collision.hpp"

#define HIT_DURATION 2.0f

class Bird : public DescriptiveAnimation, public CollisionStructure {
private :
	vcl::hierarchy_mesh_drawable bird;
	void create_body();

	float time_left_red;

public:
	Bird();

	void create();
	void animate_callback(float dt);
	void mark_hit();
	vcl::hierarchy_mesh_drawable& get_drawable();
};

namespace vcl
{
	template <typename SCENE>
	void draw(Bird& bird, SCENE const& scene)
	{
		draw(bird.get_drawable(), scene);
		draw(static_cast<CollisionStructure*>(&bird), scene);
	}
}