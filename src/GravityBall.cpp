#include "GravityBall.hpp"

GravityBall::GravityBall() 
	: radius(0.0f)
{
	particles.push_back(Particle({ 0,0,0 }, { 0,0,0 }, 0.0f));
	structure = new CollisionStructure;
}

GravityBall::~GravityBall()
{
	delete structure;
}

void GravityBall::create(float _radius) {
	radius = _radius;
	//d_mesh = vcl::mesh_primitive_sphere(radius, particles[0].position);
	d_mesh = vcl::mesh_primitive_sphere(radius, vcl::vec3({ 0,0,0 }));
	d_mesh_drawable = vcl::mesh_drawable(d_mesh);
	structure->primitive = new CollisionBall({ 0,0,0 }, radius);
	structure->create(vcl::mesh_primitive_sphere(radius));
}

void GravityBall::launch(const vcl::vec3& pos, const vcl::vec3& speed, float ttl) {
	GravityParticles::initialize_dynamics(1, pos, speed, ttl);
	//particles[0].reset(pos, speed, ttl);
	DefaultMeshDrawable::move(particles[0].position);
	structure->move_first_primitive(particles[0].position);
}


// do not forget to update collision structure position !
void GravityBall::update(float dt) {
	GravityParticles::update(dt);
	DefaultMeshDrawable::move(particles[0].position);
	structure->move_first_primitive(particles[0].position);
}

void GravityBall::kill() {
	particles[0].ttl = 0.0f;
}

bool GravityBall::isDead() {
	return particles[0].isDead();
}

vcl::vec3& GravityBall::position() {
	return particles[0].position;
}

vcl::vec3& GravityBall::speed() {
	return particles[0].speed;
}

void GravityBall::rebounce_on_surface(vcl::vec3& normal) {
	if (norm(normal) < 1e-5) speed() *= -1;
	normal /= norm(normal);
	speed() += -2.0f * vcl::dot(speed(), normal) * normal;
	speed() *= DEFAULT_GRAVITY_BALL_REBOUNCE_LOSS;
}
