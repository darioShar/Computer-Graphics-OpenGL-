#pragma once
#include <vcl/vcl.hpp>
#include "GuiWindow.hpp"

struct user_interaction_parameters {
	vcl::vec2 mouse_prev;
	vcl::timer_fps fps_record;
	vcl::mesh_drawable global_frame;
	gui_parameters gui;
	bool cursor_on_gui;
	bool capture_mouse;
	float speed;
	float time_scale;
};

vcl::vec2 get_keyboard_direction(GLFWwindow* window);
