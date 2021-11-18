#pragma once
#include <vcl/vcl.hpp>
#include <vcl/files/files.hpp>
#include "Particle.hpp"


/* Using interleaved vbo for efficiency. Updating with cpu at each frame
	Particles stored in contiguous memory for speed. Whole particle data transferred to
	gpu for shader effects.
*/

class ParticleSystem {
protected :
	int num_points;
	float ttl;
	vcl::vec3 init_pos;
	vcl::vec3 init_speed;
	std::vector<Particle> particles;


public :

	ParticleSystem();

	void initialize_dynamics(int _num_points, const vcl::vec3& pos, const vcl::vec3& speed, float ttl);
	int get_num_points();
	void update(float dt);

};
