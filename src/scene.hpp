#pragma once
#include <vcl/vcl.hpp>


struct scene_environment {
	static float aspect;
	vcl::mat4 projection;
	vcl::vec3 light;
	virtual const vcl::camera_base* get_camera() const = 0;

	// setting new projection parameters for new fov value
	void set_projection(int fov);
};

struct global_view : public scene_environment
{
	vcl::camera_around_center camera;
	const vcl::camera_base* get_camera() const;
};

struct fps_view : public scene_environment
{
	float height;
	float pitch = 0.0f;
	float yaw = 0.0f;
	vcl::camera_head camera;
	const vcl::camera_base* get_camera() const;
	void set_position_from_ground(const vcl::vec3& position);
	vcl::vec3 get_ground_position();

};

namespace vcl {
	// In order to render any scene with its camera uniform
	void opengl_uniform(GLuint shader, scene_environment const& current_scene);
}