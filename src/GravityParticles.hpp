#pragma once
#include <vcl/vcl.hpp>
#include <vcl/files/files.hpp>
#include "ParticleSystem.hpp"

class GravityParticles : public ParticleSystem {
	const float g = -9.81;

public :
	void initialize_dynamics(int _num_points, const vcl::vec3& pos, const vcl::vec3& speed, float ttl);
	void update(float dt);
	Particle& operator[](int i);
};
