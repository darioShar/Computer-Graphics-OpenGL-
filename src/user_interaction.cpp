#include "user_interaction.h"

vcl::vec2 get_keyboard_direction(GLFWwindow* window) {
	vcl::vec2 direction({ 0,0 });
	std::array<std::pair<int, vcl::vec2>, 4 > key_directions({
		std::pair<int, vcl::vec2>(GLFW_KEY_W, vcl::vec2({ 0, 1 })),
		std::pair<int, vcl::vec2>(GLFW_KEY_A, vcl::vec2({ -1, 0 })),
		std::pair<int, vcl::vec2>(GLFW_KEY_D, vcl::vec2({ 1, 0 })),
		std::pair<int, vcl::vec2>(GLFW_KEY_S, vcl::vec2({ 0, -1 }))
		});

	for (auto& key_dir : key_directions) {
		if (glfwGetKey(window, key_dir.first) == GLFW_PRESS) {
			direction += key_dir.second;
		}
	}

	float n = vcl::norm(direction);
	if (n == 0.0f) return vcl::vec2({ 0,0 });
	return direction / norm(direction);
}
