#include "GravityParticles.hpp"
#include "DefaultMeshDrawable.hpp"
#include "Collision.hpp"

#define DEFAULT_GRAVITY_BALL_MESH_SIZE 20
#define DEFAULT_GRAVITY_BALL_REBOUNCE_LOSS 0.6f

class GravityBall : public GravityParticles, public DefaultMeshDrawable
{
	float radius;
	// Make it impossible for the user to initialize more than one ball
	using GravityParticles::initialize_dynamics;

public :
	CollisionStructure* structure;

	GravityBall();
	~GravityBall();


	void create(float radius);
	void launch(const vcl::vec3& pos, const vcl::vec3& speed, float ttl);
	void update(float dt);
	void kill();
	bool isDead();
	vcl::vec3& position();
	vcl::vec3& speed();
	void rebounce_on_surface(vcl::vec3& normal);
};

namespace vcl
{
	template <typename SCENE>
	void draw(GravityBall& drawable, SCENE const& scene)
	{
		draw(static_cast<DefaultMeshDrawable>(drawable), scene);
		draw(drawable.structure, scene);
	}
}