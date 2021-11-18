#include "GravityParticles.hpp"
#include "DefaultMeshDrawable.hpp"
#include "Collision.hpp"

#define DEFAULT_GRAVITY_BALL_MESH_SIZE 20
#define DEFAULT_GRAVITY_BALL_REBOUNCE_LOSS 0.6f

class GravityBalls : public GravityParticles, public DefaultMeshDrawable
{
	float radius;
	int num_balls;
	int next_to_launch;
	// Make it impossible for the user to initialize more than one ball
	using GravityParticles::initialize_dynamics;

	void rebounce_on_surface(vcl::vec3& normal);
	Particle& get_next_to_launch();

public:
	CollisionStructure* structure;

	GravityBalls();
	~GravityBalls();


	void create(float radius, int num_balls);
	void launch(const vcl::vec3& pos, const vcl::vec3& speed, float ttl);
	void update(float dt);
	//void kill();
	//bool isDead();
	//vcl::vec3& position();
	//vcl::vec3& speed();
	std::vector<Particle>& get_particles();
};

namespace vcl
{
	template <typename SCENE>
	void draw(GravityBalls& drawable, SCENE const& scene)
	{
		for (auto& p : drawable.get_particles()) {
			if (p.isDead()) continue;
			drawable.move(p.position);
			drawable.structure->move_first_primitive(p.position);
			draw(static_cast<DefaultMeshDrawable>(drawable), scene);
			draw(drawable.structure, scene);
		}
	}
}