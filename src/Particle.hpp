#pragma once
#include <vcl/vcl.hpp>
#include <vcl/files/files.hpp>

struct Particle {
	vcl::vec3 position;
	vcl::vec3 speed;
	float ttl;

	Particle(const vcl::vec3& pos, const vcl::vec3& parent_object_speed, float ttl);


	static vcl::vec3 random_move(float norm);
	void update(float dt);
	bool isDead();
	void kill();
	void reset(const vcl::vec3& pos, const vcl::vec3& parent_object_speed, float ttl);
	void rebounce_on_surface(vcl::vec3& normal);
};
