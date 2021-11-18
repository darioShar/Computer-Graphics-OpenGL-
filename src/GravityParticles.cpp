#include "GravityParticles.hpp"



void GravityParticles::initialize_dynamics(int _num_points, const vcl::vec3& pos, const vcl::vec3& speed, float ttl) {
	ParticleSystem::initialize_dynamics(_num_points, pos, speed, ttl);
}

void GravityParticles::update(float dt) {
	for (auto& particle : particles) {
		if (particle.isDead()) continue;
		particle.update(dt);
		particle.speed += vcl::vec3({ 0,0,g }) * dt;
		particle.position += particle.speed* dt;
	}
}

Particle& GravityParticles::operator[](int i) {
	return particles[i];
}