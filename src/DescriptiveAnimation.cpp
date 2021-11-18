#include "DescriptiveAnimation.hpp"
#include "interpolation.hpp"

DescriptiveAnimation::DescriptiveAnimation()
    :   t(0.0f)
    ,   max_time(0.0f)
    ,   type(AnimationType::LinearInterpolation)
{

}

void DescriptiveAnimation::set_animation(AnimationType _type, vcl::buffer<vcl::vec3> pos, vcl::buffer<float> times) {
	type = _type;
	key_positions = pos;
	key_times = times;
    assert_vcl(pos.size() == times.size(), "Error on animation description : wrong number of points given");

    reset_time();
}

void DescriptiveAnimation::reset_time() {
    switch (type) {
    case AnimationType::LinearInterpolation :
        t = key_times[0];
        max_time = key_times[key_times.size() - 1];
        break;
    case AnimationType::CardinalSplineCatmullRom :
        t = key_times[1];
        max_time = key_times[key_times.size() - 2];
        break;
    }
}

void DescriptiveAnimation::update(float dt) {
	t += dt;
    if (t > max_time) reset_time();

	// Calculating interpolation
    // Find idx such that key_times[idx] < t < key_times[idx+1]
    int const idx = find_index_of_interval(t, key_times);

    // We must ensure that we are in right bound for cardinal spline
    if (type == AnimationType::CardinalSplineCatmullRom) {
        assert_vcl(idx > 0 && idx < key_times.size() - 2, "Error : trying to interpolate out of cardinal spline bounds");
    }
    
    // Parameters used to compute interpolation
    float const t1 = key_times[idx]; // = t_i
    float const t2 = key_times[idx + 1]; // = t_{i+1}

    vcl::vec3 const& p1 = key_positions[idx]; // = p_i
    vcl::vec3 const& p2 = key_positions[idx + 1]; // = p_{i+1}

    if (type == AnimationType::LinearInterpolation) {
        linear_interpolation(p, p_deriv, t, t1, t2, p1, p2);
    }
    else {
        float const t0 = key_times[idx - 1];
        float const t3 = key_times[idx + 2];

        vcl::vec3 const& p0 = key_positions[idx - 1];
        vcl::vec3 const& p3 = key_positions[idx + 2];

        cardinal_spline_interpolation(p, p_deriv, t, t0, t1, t2, t3, p0, p1, p2, p3, 0.5f);
    }


    if (idx == 0 || idx == key_positions.size() - 2) {
        linear_interpolation(p, p_deriv, t, t1, t2, p1, p2);
    }
    else {
        float const t0 = key_times[idx - 1];
        float const t3 = key_times[idx + 2];

        vcl::vec3 const& p0 = key_positions[idx - 1];
        vcl::vec3 const& p3 = key_positions[idx + 2];

        cardinal_spline_interpolation(p, p_deriv, t, t0, t1, t2, t3, p0, p1, p2, p3, 0.5f);
    }

    // Callback
    animate_callback(dt);
}
