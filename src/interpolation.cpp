#include "interpolation.hpp"

using namespace vcl;


void interpolation(vec3& p, vec3& p_deriv, float t, buffer<vec3> const& key_positions, buffer<float> const& key_times)
{
    // Find idx such that key_times[idx] < t < key_times[idx+1]
    int const idx = find_index_of_interval(t, key_times);

    // Parameters used to compute interpolation
    float const t1 = key_times[idx]; // = t_i
    float const t2 = key_times[idx + 1]; // = t_{i+1}

    vec3 const& p1 = key_positions[idx]; // = p_i
    vec3 const& p2 = key_positions[idx + 1]; // = p_{i+1}


    if (idx == 0 || idx == key_positions.size() - 2) {
        linear_interpolation(p, p_deriv, t, t1, t2, p1, p2);
    }
    else {
        float const t0 = key_times[idx - 1];
        float const t3 = key_times[idx + 2];

        vec3 const& p0 = key_positions[idx - 1];
        vec3 const& p3 = key_positions[idx + 2];

        cardinal_spline_interpolation(p, p_deriv, t, t0, t1, t2, t3, p0, p1, p2, p3, 0.5f);
    }
}


void linear_interpolation(vec3& p, vec3& p_deriv, float t, float t1, float t2, const vec3& p1, const vec3& p2)
{
    float const alpha = (t - t1) / (t2 - t1);
    p = (1 - alpha) * p1 + alpha * p2;
    p_deriv = p2 - p1;
}

void linear_interpolation(vec3& p, vec3& p_deriv, float t, const vec3& p1, const vec3& p2)
{
    float const alpha = t;
    p = (1 - alpha) * p1 + alpha * p2;
    p_deriv = p2 - p1;
}

void cardinal_spline_interpolation(vec3& p, vec3& p_deriv, float t, float t0, float t1, float t2, float t3, vec3 const& p0, vec3 const& p1, vec3 const& p2, vec3 const& p3, float K)
{

    float s = (t - t1) / (t2 - t1);
    vec3 d_i = 2.0f * K * (p2 - p0) / (t2 - t0);
    vec3 d_ii = 2.0f * K * (p3 - p1) / (t3 - t1);

    p = (2 * s * s * s - 3 * s * s + 1) * p1
        + (s * s * s - 2 * s * s + s) * d_i
        + (-2 * s * s * s + 3 * s * s) * p2
        + (s * s * s - s * s) * d_ii;

    p_deriv = (6 * s * s - 6 * s) * p1
        + (3 * s * s - 4 * s + 1) * d_i
        + (-6 * s * s + 6 * s) * p2
        + (3*s * s - 2 * s) * d_ii;

    p_deriv /= t2 - t1;
}

void cardinal_spline_interpolation(vec3& p, vec3& p_deriv, float t, vec3 const& p0, vec3 const& p1, vec3 const& p2, vec3 const& p3, float K)
{

    float s = t;
    vec3 d_i = 2.0f * K * (p2 - p0) / 2.0f;
    vec3 d_ii = 2.0f * K * (p3 - p1) / 2.0f;

    p = (2 * s * s * s - 3 * s * s + 1) * p1
        + (s * s * s - 2 * s * s + s) * d_i
        + (-2 * s * s * s + 3 * s * s) * p2
        + (s * s * s - s * s) * d_ii;

    p_deriv = (6 * s * s - 6 * s) * p1
        + (3 * s * s - 4 * s + 1) * d_i
        + (-6 * s * s + 6 * s) * p2
        + (3 * s * s - 2 * s) * d_ii;

}

// Assume data square
vcl::vec3 bilinear_interpolation(vcl::buffer<vcl::vec3> grid, float x, float y, int axis_size) {
    // We are not treating edge cases.
    const int N = axis_size;
    int const x0 = int(std::floor(x));
    int const y0 = int(std::floor(y));
    int const x1 = x0 + 1;
    int const y1 = y0 + 1;

    assert_vcl_no_msg(x0 >= 0 && x1 < N);
    assert_vcl_no_msg(y0 >= 0 && y1 < N);

    float const dx = x - x0;
    float const dy = y - y0;

    assert_vcl_no_msg(dx >= 0 && dx < 1);
    assert_vcl_no_msg(dy >= 0 && dy < 1);

    vec3 const v =
        (1 - dx) * (1 - dy) * grid[x0 * N + y0] +
        (1 - dx) * dy * grid[x0 * N + y1] +
        dx * (1 - dy) * grid[x1 * N + y0] +
        dx * dy * grid[x1 * N + y1];
    return v;
}


size_t find_index_of_interval(float t, buffer<float> const& intervals)
{
    size_t const N = intervals.size();
    bool error = false;
    if (intervals.size() < 2) {
        std::cout << "Error: Intervals should have at least two values; current size=" << intervals.size() << std::endl;
        error = true;
    }
    if (N > 0 && t < intervals[0]) {
        std::cout << "Error: current time t is smaller than the first time of the interval" << std::endl;
        error = true;
    }
    if (N > 0 && t > intervals[N - 1]) {
        std::cout << "Error: current time t is greater than the last time of the interval" << std::endl;
        error = true;
    }
    if (error == true) {
        std::string const error_str = "Error trying to find interval for t=" + str(t) + " within values: [" + str(intervals) + "]";
        error_vcl(error_str);
    }


    size_t k = 0;
    while (intervals[k + 1] <= t)
        ++k;
    return k;
}