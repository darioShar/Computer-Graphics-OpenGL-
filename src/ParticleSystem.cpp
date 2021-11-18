#include "ParticleSystem.hpp"

ParticleSystem::ParticleSystem() 
	:	num_points(0)
	,	ttl(0.0f)
{

}


void ParticleSystem::initialize_dynamics(int _num_points, const vcl::vec3& pos, const vcl::vec3& speed, float _ttl) {
	num_points = _num_points;
	init_pos = pos;
	init_speed = speed;
	ttl = _ttl;
	particles.resize(num_points, Particle(pos, speed, _ttl));
	for (int i = 0; i < num_points; i++) {
		//particles[i] = Particle(pos, speed, _ttl);
		particles[i].reset(pos, speed, ttl);
	}
}

void ParticleSystem::update(float dt) {

}

int ParticleSystem::get_num_points() {
	return num_points;
}
