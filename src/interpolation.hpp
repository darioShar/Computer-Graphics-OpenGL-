#pragma once

#include "vcl/vcl.hpp"

/** Find the index k such that intervals[k] < t < intervals[k+1]
* - Assume intervals is a sorted array of N time values
* - Assume t \in [ intervals[0], intervals[N-1] [       */
size_t find_index_of_interval(float t, vcl::buffer<float> const& intervals);

// Compute the interpolated position p(t) given a time t and the set of key_positions and key_frame
void interpolation(vcl::vec3& p, vcl::vec3& p_deriv, float t, vcl::buffer<vcl::vec3> const& key_positions, vcl::buffer<float> const& key_times);

/** Compute the linear interpolation p(t) between p1 at time t1 and p2 at time t2*/
void linear_interpolation(vcl::vec3& p, vcl::vec3& p_deriv, float t, float t1, float t2, const vcl::vec3& p1, const vcl::vec3& p2);

/** Compute the cardinal spline interpolation p(t) with the polygon [p0,p1,p2,p3] at time [t0,t1,t2,t3]
*  - Assume t \in [t1,t2] */
void cardinal_spline_interpolation(vcl::vec3& p, vcl::vec3& p_deriv, float t, float t0, float t1, float t2, float t3, vcl::vec3 const& p0, vcl::vec3 const& p1, vcl::vec3 const& p2, vcl::vec3 const& p3, float K);

/** Compute the linear interpolation p(t) between p1 at time 0 and p2 at time 1*/
void linear_interpolation(vcl::vec3& p, vcl::vec3& p_deriv, float t, const vcl::vec3& p1, const vcl::vec3& p2);

/** Compute the cardinal spline interpolation p(t) with the polygon [p0,p1,p2,p3] at time [-1,0,1,2]
*  - Assume t \in [0,1] */
void cardinal_spline_interpolation(vcl::vec3& p, vcl::vec3& p_deriv, float t, vcl::vec3 const& p0, vcl::vec3 const& p1, vcl::vec3 const& p2, vcl::vec3 const& p3, float K);

// bilinear interpolation [0, axis_size - 1]^2 grid
vcl::vec3 bilinear_interpolation(vcl::buffer<vcl::vec3> grid, float x, float y, int axis_size);