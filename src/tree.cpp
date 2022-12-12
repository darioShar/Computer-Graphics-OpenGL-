#include "primitive.hpp"
#include "dimensions.hpp"
#include "tree.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace vcl;

Tree::Tree()
{
    structure = new CollisionStructure;
}

Tree::~Tree() {
    delete structure;
}



void Tree::create() {
    // 0.7, 0.1
    // defining useful constants for tree construction
    const float h = dimensions::tree.height * 0.7f / 1.7f;
    const float r = dimensions::tree.radius / 4.0f;

    // Build hierarchy with one bigger englobing sphere
    vec3 englobing_ball_position({ 0,0, dimensions::tree.height / 2.0f });
    float englobing_ball_radius = dimensions::tree.height / 1.5f;
    structure->primitive = new CollisionBall(englobing_ball_position, englobing_ball_radius);
    // Creating hit box
    structure->create(mesh_primitive_sphere(englobing_ball_radius));

    // Create a brown trunk
    mesh trunk = create_cylinder(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });

    CollisionStructure* sub_struct_trunk = new CollisionStructure();
    vec3 p000({ -r, - r, 0 });
    vec3 p100({ 2 * r, 0 ,0 });
    vec3 p010({ 0, 2 * r ,0 });
    vec3 p001({ 0, 0 ,h });

    sub_struct_trunk->primitive = new CollisionCuboid(p000 - englobing_ball_position,
                                    p100, p010, p001);
    
    // Creating hit box
    //sub_struct_trunk->create(mesh_primitive_cubic_grid(p000, p000 + p100, p000 + p100 + p010, p000 + p010, p000 + p001, p000 + p100 + p001, p000 + p100 + p010 + p001, p000 + p010 + p001));
    sub_struct_trunk->create(mesh_primitive_cubic_grid(vec3({ 0,0,0 }), p100, p100 + p010, p010, p001, p100 + p001, p100 + p010 + p001, p010 + p001));

    // Create a green foliage from 3 cones
    mesh foliage = create_cone(4 * r, 6 * r, 0.0f);      // base-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 2 * r));   // middle-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 4 * r));   // top-cone
    foliage.position += vec3(0, 0, h);                 // place foliage at the top of the trunk
    foliage.color.fill({ 0.4f, 0.6f, 0.3f });

    // hit boxes
    p000 = vec3({ -4 * r, -4 * r, h });
    p100 = vec3({ 8 * r, 0 ,0 });
    p010 = vec3({ 0, 8 * r ,0 });
    p001 = vec3({ 0, 0 ,6 * r });
    CollisionStructure* sub_struct_foliage_0 = new CollisionStructure();
    sub_struct_foliage_0->primitive = new CollisionCuboid(p000 - englobing_ball_position,
        p100, p010, p001);
    // Creating hit box. Will be displaced to primtive's pos
    //sub_struct_foliage_0->create(mesh_primitive_cubic_grid(p000, p000 + p100, p000 + p100 + p010, p000 + p010, p000 + p001, p000 + p100 + p001, p000 + p100 + p010 + p001, p000 + p010 + p001));
    sub_struct_foliage_0->create(mesh_primitive_cubic_grid(vec3({ 0,0,0 }), p100, p100 + p010, p010, p001, p100 + p001, p100 + p010 + p001, p010 + p001));


    p000 = vec3({ -4 * r, -4 * r, h + 2 *r });
    p100 = vec3({ 8 * r, 0 ,0 });
    p010 = vec3({ 0, 8 * r ,0 });
    p001 = vec3({ 0, 0 ,6 * r });
    CollisionStructure* sub_struct_foliage_1 = new CollisionStructure();
    sub_struct_foliage_1->primitive = new CollisionCuboid(p000 - englobing_ball_position,
        p100, p010, p001);
    // Creating hit box
    //sub_struct_foliage_1->create(mesh_primitive_cubic_grid(p000, p000 + p100, p000 + p100 + p010, p000 + p010, p000 + p001, p000 + p100 + p001, p000 + p100 + p010 + p001, p000 + p010 + p001));
    sub_struct_foliage_1->create(mesh_primitive_cubic_grid(vec3({ 0,0,0 }), p100, p100 + p010, p010, p001, p100 + p001, p100 + p010 + p001, p010 + p001));


    p000 = vec3({ -4 * r, -4 * r, h + 4 * r });
    p100 = vec3({ 8 * r, 0 ,0 });
    p010 = vec3({ 0, 8 * r ,0 });
    p001 = vec3({ 0, 0 ,6 * r });
    CollisionStructure* sub_struct_foliage_2 = new CollisionStructure();
    sub_struct_foliage_2->primitive = new CollisionCuboid(p000 - englobing_ball_position,
        p100, p010, p001);
    // Creating hit box
    //sub_struct_foliage_2->create(mesh_primitive_cubic_grid(p000, p000 + p100, p000 + p100 + p010, p000 + p010, p000 + p001, p000 + p100 + p001, p000 + p100 + p010 + p001, p000 + p010 + p001));
    sub_struct_foliage_2->create(mesh_primitive_cubic_grid(vec3({ 0,0,0 }), p100, p100 + p010, p010, p001, p100 + p001, p100 + p010 + p001, p010 + p001));

    structure->subStructures.push_back(sub_struct_trunk);
    structure->subStructures.push_back(sub_struct_foliage_0);
    structure->subStructures.push_back(sub_struct_foliage_1);
    structure->subStructures.push_back(sub_struct_foliage_2);


    // ok

    // The tree is composed of the trunk and the foliage
    d_mesh = trunk;
    d_mesh.push_back(foliage);

    d_mesh.fill_empty_field(); 
    // Now uploading data to gpu
    d_mesh_drawable = mesh_drawable(d_mesh);
}

void Tree::move(vcl::vec3 v) {
    // We must translate drawable and translate collision structure !
    DefaultMeshDrawable::move(v);
    structure->move_first_primitive(v);
}
