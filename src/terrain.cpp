#include "dimensions.hpp"
#include "terrain.hpp"
#include "interpolation.hpp"

using namespace vcl;

float eval_terrain_func(vec2 point, vec2 center, float h, float s) {
    float d = norm(point - center) / s;
    return h * std::exp(-d * d);
}

float x_stretch(float u, float side_length) {
    return 2 * side_length * (u - 0.5f);
}

float reverse_x_stretch(float x, float side_length) {
    return 0.5f + (x / (2.0f * side_length));
}

float y_stretch(float v, float side_length) {
    return 2 * side_length * (v - 0.5f);
}

float reverse_y_stretch(float y, float side_length) {
    return 0.5f + (y / (2.0f * side_length));
}

Terrain::Terrain(float _side_length, int _mesh_size, perlin_noise_parameters _perlin)
    :   side_length(_side_length)
    ,   mesh_size(_mesh_size)
    ,   perlin(_perlin)
{

}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vcl::vec3 Terrain::evaluate_terrain(float u, float v)
{
    float const x = x_stretch(u, side_length);
    float const y = y_stretch(v, side_length);
    std::vector<vec2> centers({ vec2(0.0f, 0.0f), vec2(0.5f, 0.5f), vec2(0.2f, 0.7f), vec2(0.8f, 0.7f) });
    std::vector<float> heights({ 5.0f, -3.0f, 4.0f, 3.0f });
    std::vector<float> stdevs({ 0.5f, 0.15f, 0.2f, 0.2f });

    float z = 0.0f;
    for (int i = 0; i < centers.size(); i++)
        z += eval_terrain_func(vec2(u, v), centers[i], heights[i], stdevs[i]);

    return { x,y,z };
}


void Terrain::create(float _side_length, int _mesh_size)
{
    mesh_size = _mesh_size;
    side_length = _side_length;

    const int N = mesh_size;
    d_mesh.position.resize(N * N);
    d_mesh.uv.resize(N * N);

    // Fill terrain geometry
    for(unsigned int ku=0; ku<N; ++ku)
    {
        for(unsigned int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute the local surface function
            // vec3 const p = evaluate_terrain(u,v);
            // Just here for  first
            vec3 const p(x_stretch(u, side_length), y_stretch(v, side_length), 0.0f);

            // Store vertex coordinates
            d_mesh.position[kv+N*ku] = p;
            d_mesh.uv[kv + N * ku] = { 10.0f * (float)ku / (float)(N - 1) , 10.0f * (float)kv / (float)(N - 1) };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(size_t ku=0; ku<N-1; ++ku)
    {
        for(size_t kv=0; kv<N-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+N, idx+1};
            const uint3 triangle_2 = {idx, idx+N, idx+1+N};

            d_mesh.connectivity.push_back(triangle_1);
            d_mesh.connectivity.push_back(triangle_2);
        }
    }

    d_mesh.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)

    // Now uploading data to gpu
    d_mesh_drawable = mesh_drawable(d_mesh);


    // We use collision structure only for the box surronding the terrain. For the ground we'll use
    // the real height. Anyway, first primitive must always trigger intersection in order to go
    // into the four walls
    /*primitive = new CollisionBall({ 0,0,0 }, std::sqrt(2.0f) * 2 * side_length + 1.0f);

    CollisionStructure* side_0 = new CollisionStructure();
    side_0->primitive = new CollisionPlane(vcl::vec3({ -side_length + 1.0f,0,0 }), vcl::vec3({ 1,0,0 }));

    CollisionStructure* side_1 = new CollisionStructure();
    side_1->primitive = new CollisionPlane(vcl::vec3({ side_length - 1.0f,0,0 }), vcl::vec3({ -1,0,0 }));

    CollisionStructure* side_2 = new CollisionStructure();
    side_2->primitive = new CollisionPlane(vcl::vec3({ 0,-side_length + 1.0f ,0 }), vcl::vec3({ 0,1,0 }));

    CollisionStructure* side_3 = new CollisionStructure();
    side_3->primitive = new CollisionPlane(vcl::vec3({ 0,side_length - 1.0f,0 }), vcl::vec3({0,-1,0 }));

    subStructures.push_back(side_0);
    subStructures.push_back(side_1);
    subStructures.push_back(side_2);
    subStructures.push_back(side_3);*/

}

vcl::vec3 Terrain::get_normalized_terrain_height(float u, float v) {
    // evaluate vertex index
    const int N = std::sqrt(d_mesh.position.size());
    const int ku = (N - 1) * u;
    const int kv = (N - 1) * v;
    const int index = kv + N * ku;

    return d_mesh.position[index];
}

vcl::vec3 Terrain::get_true_terrain_height(float x, float y) {
    return get_normalized_terrain_height(reverse_x_stretch(x, side_length), reverse_y_stretch(y, side_length));
}

vcl::vec3 Terrain::get_bilinear_interpolated_terrain_height(float x, float y) {
    const int N = std::sqrt(d_mesh.position.size());
    x = reverse_x_stretch(x, side_length) * (N - 1);
    y = reverse_y_stretch(y, side_length) * (N - 1);
    return bilinear_interpolation(d_mesh.position, x, y, N);
}

vcl::vec3 Terrain::get_spline_interpolated_terrain_height(float x, float y) {
    // doing some interpolation to walk on terrain.

    // find normalized coordinates
    float u = reverse_x_stretch(x, side_length);
    float v = reverse_y_stretch(y, side_length);

    // evaluate vertex index
    const int N = std::sqrt(d_mesh.position.size());
    int ku = (N - 1) * u;
    int kv = (N - 1) * v;
    int index = kv + N * ku;

    vec3 point00(d_mesh.position[index]);

    // Intermediary points for bilin calculation
    vec3 point_lin_0, point_lin_0_der;
    vec3 point_lin_1, point_lin_1_der;
    vec3 point_bilin, point_bilin_der;

    // we are bottom left of the quadrangle containing the point.
    // edge cases : rightmost u
    if (ku >= N - 1) {
        ku = N - 1;
        if (kv >= N - 1) return point00;
        // ...
        // ...
        // ...
        // ...
        // ...
        // ...
    }

    // ok now normal case. Extremal points :
    vec3 point01(d_mesh.position[index + 1]);
    vec3 point10(d_mesh.position[index + N]);
    vec3 point11(d_mesh.position[index + 1 + N]);

    // Calculating
    linear_interpolation(point_lin_0, point_lin_0_der, u - (float)(ku / (N - 1)), point00, point10);
    linear_interpolation(point_lin_1, point_lin_1_der, u - (float)(ku / (N - 1)), point01, point11);
    linear_interpolation(point_bilin, point_bilin_der, v - (float)(kv / (N - 1)), point_lin_0, point_lin_1);

    //ok

    return point_bilin;
}

vcl::vec3 Terrain::get_normal(float x, float y) {
    float u = reverse_x_stretch(x, side_length);
    float v = reverse_y_stretch(y, side_length);
    int ku = u * (mesh_size - 1);
    int kv = v * (mesh_size - 1);
    return d_mesh.normal[kv + mesh_size * ku];
}


perlin_noise_parameters& Terrain::perlin_param() {
    return perlin;
}


void Terrain::deform_terrain_perlin() {
    // Number of samples in each direction (assuming a square grid)
    int const N = std::sqrt(d_mesh.position.size());

    // Recompute the new vertices
    for (int ku = 0; ku < N; ++ku) {
        for (int kv = 0; kv < N; ++kv) {

            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            int const idx = ku * N + kv;

            // Compute the Perlin noise
            float const noise = noise_perlin({ u, v }, perlin.octave, perlin.persistency, perlin.frequency_gain);

            // update with noise
            d_mesh.position[idx].z += perlin.terrain_height * (noise * 2.0f - 1.0f);

            // use also the noise as color value
            //terrain.color[idx] = 0.3f * vec3(0, 0.5f, 0) + 0.7f * noise * vec3(1, 1, 1);
        }
    }

    // Update the normal of the mesh structure
    d_mesh.compute_normal();

    // and update gpu data
    update_mesh_drawable();
}

bool Terrain::point_intersects_ground(vcl::vec3& point) {
    return point.z < get_bilinear_interpolated_terrain_height(point.x, point.y).z;
}

bool Terrain::bounce_on_sides(vcl::vec3& position, vcl::vec3& speed, float dist) {
    float x = std::min(position.x + side_length, side_length - position.x);
    float y = std::min(position.y + side_length, side_length - position.y);
    float z = std::min(position.z + side_length, side_length - position.z);
    float min = std::min(x, std::min(y, z));

    // if too far from edges
    if (min > dist) return false;
    // else : collision
    if (min == x) {
        speed.x *= -1;
        if (x == position.x + side_length)
            position.x = -side_length + dist;
        else 
            position.x =  side_length - dist;
    }
    else if (min == y) {
        speed.y *= -1;
        if (y == position.y + side_length)
            position.y = -side_length + dist;
        else
            position.y = side_length - dist;
    }
    else {
        speed.z *= -1;
        if (z == position.z + side_length)
            position.z = -side_length + dist;
        else
            position.z = side_length - dist;
    }
    return true;
}

float Terrain::distance_to_sides(vcl::vec3& point) {
    float x = std::min(point.x + side_length, side_length - point.x);
    float y = std::min(point.y + side_length, side_length - point.y);
    float z = std::min(point.z + side_length, side_length - point.z);
    return std::min(x, std::min(y, z ));
}