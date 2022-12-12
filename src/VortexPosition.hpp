#pragma once
#include "DescriptiveAnimation.hpp"

void random_vortex_move(vcl::vec3& vortex_pos, float speed, float dt) {
	vortex_pos.x += 2.0f * ((float)rand() / (float)RAND_MAX - 0.5f) * speed * dt;
	vortex_pos.y += 2.0f * ((float)rand() / (float)RAND_MAX - 0.5f) * speed * dt;
}

class VortexPosition : public DescriptiveAnimation {
public :
	vcl::vec3& get_position() {
		return p;
	}

	void animate_callback(float dt) {

	}
};