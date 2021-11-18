#pragma once
#include <vcl/vcl.hpp>
#include <vcl/files/files.hpp>
#include "ParticleSystem.hpp"

#define PARTICLE_SPEED_DRAG 0.0f
#define PARTICLE_TIME_UNCERTAINTY 0.5f

#define SMOKE_DEFAULT_SIZE 8.0f
#define SMOKE_COLOR vcl::vec3({1,1,1})/5.0f;
#define SMOKE_ATTENUATION vcl::vec3({1,0,0.05});


/* Using interleaved vbo for efficiency. Updating with cpu at each frame
	Particles stored in contiguous memory for speed. Whole particle data transferred to
	gpu for shader effects.
*/

class Smoke  : public ParticleSystem {
	GLuint vbo;
	GLuint vao;
	GLuint shader;
	GLuint texture;

	void setup_gpu_data();
	void update_smoke_particle(Particle& particle, float dt);
	void modulate_smoke_particle_reset(Particle& particle);

	const vcl::vec3* pos_to_follow;

public:

	static float vorticity;
	static float randomness;
	static float initial_offset;

	// Uniform
	struct {
		vcl::vec3 color; // initial color, will go on top of texture
		vcl::vec3 attenuation_coeffs; // attenuation coefficients when getting away from particles
		float default_size; // initial particle size at d = 0
		float ttl; // start time to live. each particle will have ttl-- during evolution
	}shading;

	Smoke();

	// Will follow point pos, smoke ejected at speed speed
	void initialize_dynamics(int _num_points, const vcl::vec3* pos, const vcl::vec3& speed, float ttl);

	void setup(const std::string& shader_vert, const std::string& shader_frag, const std::string& texture);

	void update(float dt);

	template <typename SCENE>
	friend void draw(Smoke& smoke, SCENE const& scene);

};


template <typename SCENE>
void draw(Smoke& smoke, SCENE const& scene)
{
	// Setup shader
	assert_vcl(smoke.shader != 0, "Try to draw mesh_drawable without shader");
	assert_vcl(smoke.texture != 0, "Try to draw mesh_drawable without texture");
	glUseProgram(smoke.shader); opengl_check;

	// Default size, will be attenuated wrt distance (see shader)
	glPointSize(SMOKE_DEFAULT_SIZE);

	// Max and min size
	glPointParameterf(GL_POINT_SIZE_MAX, SMOKE_DEFAULT_SIZE);
	glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);

	// Set origin of gl_PointCoord to lower left
	//glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

	// Enabling round points
	//glEnable(GL_POINT_SMOOTH);
	// but keeping speed
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);

	// Enabling gl_PointSize in vertex shader
	glEnable(GL_PROGRAM_POINT_SIZE); opengl_check;

	// Send uniforms for this shader
	opengl_uniform(smoke.shader, "projection", scene.projection);
	opengl_uniform(smoke.shader, "view", scene.get_camera()->matrix_view());
	opengl_uniform(smoke.shader, "default_size", smoke.shading.default_size);
	opengl_uniform(smoke.shader, "coeffs", smoke.shading.attenuation_coeffs);
	opengl_uniform(smoke.shader, "total_time", smoke.shading.ttl);
	//opengl_uniform(smoke.shader, "color", smoke.shading.color);

	// Set texture
	glActiveTexture(GL_TEXTURE0); opengl_check;
	glBindTexture(GL_TEXTURE_2D, smoke.texture); opengl_check;
	opengl_uniform(smoke.shader, "image_texture", 0);  opengl_check;

	// Call draw function
	assert_vcl(smoke.get_num_points() > 0, "Try to draw particles with 0 points"); opengl_check;
	glBindVertexArray(smoke.vao);   opengl_check;
	glDrawArrays(GL_POINTS, 0, smoke.get_num_points());

	// Clean buffers
	// ALWAYS UNBIND VAO FIRST
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}