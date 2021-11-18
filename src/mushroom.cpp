#include "primitive.hpp"
#include "dimensions.hpp"
#include "mushroom.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace vcl;

vcl::mesh create_mushroom() {
    float const h = dimensions::mushroom.height * 0.8f;
    float const r = dimensions::mushroom.radius / 3.0f;
    
    mesh trunk = create_cylinder(r, h);
    vec3 color(244.0f, 240.0f, 225.0f);
    color /= 255.0f;
    trunk.color.fill(color);


    // Create mushroom top
    mesh top = create_cone(3 * r, dimensions::mushroom.height - h, h);      // base-cone
    color = vec3(255.0f, 73.0f, 53.0f);
    color /= 255.0f;
    top.color.fill(color);

    // Assemble
    mesh tree = trunk;
    tree.push_back(top);

    return tree;
}
