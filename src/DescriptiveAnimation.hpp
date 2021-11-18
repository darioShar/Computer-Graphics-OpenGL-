#pragma once
#include <vcl/vcl.hpp>

enum class AnimationType {
	LinearInterpolation,
	CardinalSplineCatmullRom
};

class DescriptiveAnimation {
protected :
	vcl::vec3 p;
	vcl::vec3 p_deriv;
	float t;
	float max_time;

	// animation positions
	AnimationType type;
	vcl::buffer<vcl::vec3> key_positions;
	vcl::buffer<float> key_times;

	void reset_time();

public :
	DescriptiveAnimation();
	void set_animation(AnimationType _type, vcl::buffer<vcl::vec3> pos, vcl::buffer<float> times);
	void update(float dt);

	virtual void animate_callback(float dt) = 0;
};