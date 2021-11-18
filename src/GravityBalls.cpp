#include "GravityBalls.hpp"

GravityBalls::GravityBalls()
	: radius(0.0f)
	, num_balls(0)
	, next_to_launch(0)
{
	//particles.resize(Particle({ 0,0,0 }, { 0,0,0 }, 0.0f));
	structure = new CollisionStructure;
}

GravityBalls::~GravityBalls()
{
	delete structure;
}

void GravityBalls::create(float _radius, int _num_balls) {
	radius = _radius;
	num_balls = _num_balls;
	d_mesh = vcl::mesh_primitive_sphere(radius, vcl::vec3({ 0,0,0 }));
	d_mesh_drawable = vcl::mesh_drawable(d_mesh);
	structure->primitive = new CollisionBall({ 0,0,0 }, radius);
	structure->create(vcl::mesh_primitive_sphere(radius + 0.01f));
	GravityParticles::initialize_dynamics(num_balls, vcl::vec3({ 0,0,0 }), vcl::vec3({ 0,0,0 }), 0);

}

Particle& GravityBalls::get_next_to_launch() {
	return (*this)[next_to_launch];
}


void GravityBalls::launch(const vcl::vec3& pos, const vcl::vec3& speed, float ttl) {
	//particles[0].reset(pos, speed, ttl);
	get_next_to_launch().reset(pos, speed, ttl);
	next_to_launch = (next_to_launch + 1) % num_balls;
	//DefaultMeshDrawable::move(particles[0].position);
	structure->move_first_primitive(particles[0].position);
}


// do not forget to update collision structure position !
void GravityBalls::update(float dt) {
	GravityParticles::update(dt);
}

/*void GravityBalls::kill() {
	particles[0].ttl = 0.0f;
}

bool GravityBalls::isDead() {
	return particles[0].isDead();
}

vcl::vec3& GravityBalls::position() {
	return particles[0].position;
}

vcl::vec3& GravityBalls::speed() {
	return particles[0].speed;
}
*/

/*
void GravityBalls::rebounce_on_surface(vcl::vec3& normal) {
	if (norm(normal) < 1e-5) speed() *= -1;
	normal /= norm(normal);
	speed() += -2.0f * vcl::dot(speed(), normal) * normal;
	speed() *= DEFAULT_GRAVITY_BALL_REBOUNCE_LOSS;
}
*/

std::vector<Particle>& GravityBalls::get_particles() {
	return particles;
}
