#pragma once

#include "vcl/vcl.hpp"
#include "DefaultMeshDrawable.hpp"
#include "Collision.hpp"

#define DEFAULT_TERRAIN_MESH_SIDE 200

struct perlin_noise_parameters
{
	float persistency = 0.4f;
	float frequency_gain = 2.4f;
	int octave = 8;
	float terrain_height = 2.0f;
};

class Terrain : public DefaultMeshDrawable {
private :
	int mesh_size;
	float side_length;
	
	perlin_noise_parameters perlin;
	vcl::vec3 evaluate_terrain(float u, float v);


public :
	Terrain(float side_length = 0.0f, int mesh_size = DEFAULT_TERRAIN_MESH_SIDE, perlin_noise_parameters perlin = perlin_noise_parameters());

	void create(float side_length, int mesh_size = DEFAULT_TERRAIN_MESH_SIDE);

	// get terrain position for normal coordinate in [0,1]^2. 
	// Actual terrain elevation, even after perlin noise etc.
	vcl::vec3 get_normalized_terrain_height(float u, float v);

	// Getting x and y back to normalized coordinates
	vcl::vec3 get_true_terrain_height(float x, float y);

	// Doing interpolation calculation
	vcl::vec3 get_bilinear_interpolated_terrain_height(float x, float y);

	// Doing interpolation calculation
	vcl::vec3 get_spline_interpolated_terrain_height(float x, float y);

	// Getting normal on x, y coordinate.
	vcl::vec3 get_normal(float x, float y);

	perlin_noise_parameters& perlin_param();

	// Deform terrain using perlin noise
	void deform_terrain_perlin();

	bool point_intersects_ground(vcl::vec3& point);
	bool bounce_on_sides(vcl::vec3& point, vcl::vec3& speed, float dist);
	float distance_to_sides(vcl::vec3& point);


};
