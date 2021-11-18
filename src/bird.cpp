#include "bird.h"
#include "interpolation.hpp"

Bird::Bird() : DescriptiveAnimation(), time_left_red(0.0f) {
	// We can't create in constructor because we have to wait for opengl to be enabled
}

void Bird::create_body() {
	// Definition of the elements of the hierarchy
	// ------------------------------------------- //

	float const radius_body = 0.25f;
	float const radius_head = radius_body * 0.8f;
	float const length_arm = radius_body;
	float const length_beak = radius_body * 0.4f;

	using namespace vcl;
	// The geometry of the body is an ellipsoid
	vec3 const dim_body = { 1.7f * radius_body, radius_body, radius_body };
	mesh_drawable body = mesh_drawable(mesh_primitive_ellipsoid(dim_body, { 0,0,0 }, 40, 40));

	// head
	mesh_drawable head = mesh_drawable(mesh_primitive_sphere(radius_head, { 0,0,0 }, 20, 20));

	// Geometry of the eyes: black spheres	
	mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(radius_head / 5.0f, { 0,0,0 }, 20, 20));
	eye.shading.color = { 0,0,0 };

	// Shoulder part and arm
	vec3 p00 = { -length_arm / 2.0f, -length_arm, 0.0f };
	vec3 p10 = { length_arm / 2.0f, -length_arm, 0.0f };
	vec3 p11 = { length_arm / 2.0f, 0.0f, 0.0f };
	vec3 p01 = { -length_arm / 2.0f, 0.0f, 0.0f };
	mesh_drawable wing_right = mesh_drawable(mesh_primitive_quadrangle(p00, p10, p11, p01));
	p10 -= {length_arm / 4.0f, 0.0f, 0.0f};
	p00 += {length_arm / 3.0f, 0.0f, 0.0f};

	mesh_drawable extremity_right = mesh_drawable(mesh_primitive_quadrangle(p00, p10, p11, p01));

	// reset
	p10 += {length_arm / 4.0f, 0.0f, 0.0f};
	p00 -= {length_arm / 3.0f, 0.0f, 0.0f};

	// translate
	p00 += {0.0f, length_arm, 0.0f};
	p10 += {0.0f, length_arm, 0.0f};
	p11 += {0.0f, length_arm, 0.0f};
	p01 += {0.0f, length_arm, 0.0f};
	mesh_drawable wing_left = mesh_drawable(mesh_primitive_quadrangle(p00, p10, p11, p01));
	p11 -= {length_arm / 4.0f, 0.0f, 0.0f};
	p01 += {length_arm / 3.0f, 0.0f, 0.0f};
	mesh_drawable extremity_left = mesh_drawable(mesh_primitive_quadrangle(p00, p10, p11, p01));


	// A beak
	mesh_drawable beak = mesh_drawable(mesh_primitive_cone(length_beak * 0.8f, length_beak, { 0, 0, 0 }, { 1, 0, 0 }));
	beak.shading.color = { 1.0f, 0.5f, 0.0f }; // orange

	// Build the hierarchy:
	// ------------------------------------------- //
	// Syntax to add element
	//   hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])

	// The root of the hierarchy is the body
	bird.add(body, "body");

	// head
	float const angle = pi / 6.0f;
	bird.add(head, "head", "body", { dim_body.x * std::cosf(angle), 0.0f, dim_body.z * std::sinf(angle) });

	// Eyes positions are set with respect to some ratio of the body
	bird.add(eye, "eye_left", "head", radius_head * vec3(1.0f / 1.5f, 1 / 3.0f, 1 / 2.0f));
	bird.add(eye, "eye_right", "head", radius_head * vec3(1.0f / 1.5f, -1 / 3.0f, 1 / 2.0f));

	// beak
	bird.add(beak, "beak", "head", { radius_head, 0, 0 });

	// Set the left part of the body arm
	bird.add(wing_left, "wing_left", "body", { 0.0f,dim_body.y,0 }); // extremity of the spherical body
	bird.add(extremity_left, "extremity_left", "wing_left", { 0,length_arm,0 });          // place the elbow the extremity of the "shoulder cylinder"

	// Set the right part of the body arm: similar to the left part with a symmetry in x direction
	bird.add(wing_right, "wing_right", "body", { 0,-dim_body.y,0 });
	bird.add(extremity_right, "extremity_right", "wing_right", { 0, -length_arm,0 });


	primitive = new CollisionBall({ 0,0,0 }, dim_body.x * 1.2f);
	CollisionStructure::create(mesh_primitive_sphere(dim_body.x * 1.2f));
}

void Bird::create() {
	create_body();
}

void Bird::animate_callback(float dt) {
	// Now calculating rotation
	p_deriv /= norm(p_deriv);
	vcl::vec3 p_actual = vcl::vec3({ 1,0,0 });
	vcl::vec3 cp = cross(p_actual, p_deriv); // ATTENTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	float theta = std::acos(dot(p_deriv, p_actual));
	if (norm(cp) > 1e-7) {
		cp /= norm(cp);
		bird["body"].transform.rotate = vcl::rotation(cp, theta);
	}
	else {
		if (p_deriv[0] < 0.0f) bird["body"].transform.rotate = vcl::rotation({ 0,0,1 }, vcl::pi);
		else  bird["body"].transform.rotate = vcl::rotation();
	}
	//std::cout <<  p_deriv << " , " << cp << " : " << theta << std::endl;

	p -= {1, 1, 0};
	p *= 2.0f;
	p[2] += 3.0f;

	bird["body"].transform.translate = p;

	float speed = norm(p_deriv);

	// head
	bird["head"].transform.rotate = vcl::rotation({ 0,-1,0 }, 0.4f + 0.3f * std::sin(1.1 * 2 * vcl::pi * (speed * t - 0.2)));

	// Rotation of the shoulder-left around the y axis
	bird["wing_left"].transform.rotate = vcl::rotation({ 1,0,0 }, 0.7f * std::sin(2 * 2 * vcl::pi * (speed * t)));
	// Rotation of the arm-left around the y axis (delayed with respect to the shoulder)
	bird["extremity_left"].transform.rotate = vcl::rotation({ 1,0,0 }, std::sin(2 * 2 * vcl::pi * (speed * t)));

	// Rotation of the shoulder-right around the y axis
	bird["wing_right"].transform.rotate = vcl::rotation({ -1,0,0 }, 0.7f * std::sin(2 * 2 * vcl::pi * (speed * t)));
	// Rotation of the arm-right around the y axis (delayed with respect to the shoulder)
	bird["extremity_right"].transform.rotate = vcl::rotation({ -1,0,0 }, std::sin(2 * 2 * vcl::pi * (speed * t)));

	// update the global coordinates
	bird.update_local_to_global_coordinates();

	// update primitive
	move_first_primitive(p);

	// if hit
	time_left_red -= dt;
	if (time_left_red >= 0.0f) {
		const size_t N = bird.elements.size();
		for (size_t k = 0; k < N; ++k)
		{
			vcl::hierarchy_mesh_drawable_node& node = bird.elements[k];
			float val = time_left_red / HIT_DURATION;
			node.element.shading.color.x = 1.0f;
			node.element.shading.color.y = 1.0f - val;
			node.element.shading.color.z = 1.0f - val;
		}
	}
}

void Bird::mark_hit() {
	time_left_red = HIT_DURATION;
}

vcl::hierarchy_mesh_drawable& Bird::get_drawable() {
	return bird;
}
