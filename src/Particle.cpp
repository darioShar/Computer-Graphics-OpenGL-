#include "Particle.hpp"


Particle::Particle(const vcl::vec3& pos, const vcl::vec3& parent_object_speed, float _ttl) 
	:	position(pos) 
	,	speed(parent_object_speed)
	,	ttl(_ttl)
{

}


vcl::vec3 Particle::random_move(float norm) {
	float theta = std::acosf( 2.0f * (float)rand() / (float)RAND_MAX - 1);
	float phi = 2.0f * vcl::pi* (float)rand() / (float)RAND_MAX;
	vcl::vec3 random_vec({ 0,0,0 });
	random_vec.x += std::sinf(theta) * std::cos(phi);
	random_vec.y += std::sinf(theta) * std::sin(phi);
	random_vec.z += std::cosf(theta);
	return random_vec * norm;
}

void Particle::update(float dt) {
	ttl -= dt;
}


bool Particle::isDead() {
	return ttl <= 0.0f;
	//return true;
}

void Particle::kill() {
	ttl = 0.0f;
}


void Particle::reset(const vcl::vec3& pos, const vcl::vec3& parent_object_speed, float _ttl) {
	position = pos;
	speed = parent_object_speed;
	ttl = _ttl;
}

void Particle::rebounce_on_surface(vcl::vec3& normal) {
	if (norm(normal) < 1e-5) speed *= -1;
	normal /= norm(normal);
	speed += -2.0f * vcl::dot(speed, normal) * normal;
}
