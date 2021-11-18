#include "scene.hpp"

namespace vcl {
	void opengl_uniform(GLuint shader, scene_environment const& current_scene)
	{
		opengl_uniform(shader, "projection", current_scene.projection);
		opengl_uniform(shader, "view", current_scene.get_camera()->matrix_view());
		opengl_uniform(shader, "light", current_scene.light, false);
	}
}

void scene_environment::set_projection(int fov) {
	projection = vcl::projection_perspective(static_cast<float>(fov) * vcl::pi / 180.0f, aspect, 0.2f, 10000.0f);
}

const vcl::camera_base* global_view::get_camera() const { return &camera; }
const vcl::camera_base* fps_view::get_camera() const { return &camera; }

void fps_view::set_position_from_ground(const vcl::vec3& position) {
	camera.position_camera = position;
	camera.position_camera[2] += height;
}

vcl::vec3 fps_view::get_ground_position() {
	vcl::vec3 pos = camera.position_camera;
	pos.z -= height;
	return pos;
}