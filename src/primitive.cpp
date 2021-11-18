#include "primitive.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace vcl;

mesh create_cylinder(float radius, float height)
{
    mesh m;

    const unsigned int circle_sampling_size = 20;

    const unsigned int N = 2 * circle_sampling_size;

    m.position.resize(N);

    // Fill tree geometry
    for (unsigned int i = 0; i < circle_sampling_size; ++i)
    {
        const float u = radius * std::cos(2 * M_PI * i / circle_sampling_size);
        const float v = radius * std::sin(2 * M_PI * i / circle_sampling_size);

        // Store vertex coordinates
        m.position[2 * i] = vec3(u, v, 0.0f);
        m.position[2 * i + 1] = vec3(u, v, height);
    }

    for (unsigned int i = 0; i < circle_sampling_size - 1; ++i)
    {
        m.connectivity.push_back({ 2 * i, 2 * i + 1, 2 * i + 3 });
        m.connectivity.push_back({ 2 * i, 2 * i + 3, 2 * i + 2 });
    }
    m.connectivity.push_back({ N - 2, N - 1, 1 });
    m.connectivity.push_back({ N - 2, 1, 0 });

    m.fill_empty_field();
    return m;
}

mesh create_cone(float radius, float height, float z_offset)
{
    mesh m;

    const unsigned int circle_sampling_size = 20;
    const unsigned int N = circle_sampling_size + 2;
    m.position.resize(N);

    // Fill tree geometry
    for (unsigned int i = 0; i < circle_sampling_size; ++i)
    {
        // Compute local parametric coordinates (u,v) \in [0,1]
        const float u = radius * std::cos(2 * M_PI * i / circle_sampling_size);
        const float v = radius * std::sin(2 * M_PI * i / circle_sampling_size);

        // Store vertex coordinates
        m.position[i] = vec3(u, v, z_offset);
    }
    m.position[circle_sampling_size] = vec3(0.0f, 0.0f, z_offset);
    m.position[circle_sampling_size + 1] = vec3(0.0f, 0.0f, height + z_offset);


    for (unsigned int i = 0; i < circle_sampling_size - 1; ++i)
    {
        m.connectivity.push_back({ i, circle_sampling_size + 1, i + 1 });
        m.connectivity.push_back({ i, i + 1, circle_sampling_size });
    }
    m.connectivity.push_back({ circle_sampling_size - 1, circle_sampling_size + 1, 0 });
    m.connectivity.push_back({ circle_sampling_size - 1, 0, circle_sampling_size });

    m.fill_empty_field();
    return m;
}
