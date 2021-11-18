#include "Collision.hpp"


bool test_collision_1() {
	// Testing collision
	/*CollisionBall* ball1 = new CollisionBall(vcl::vec3({ 0,0,0 }), 1.0f);
	CollisionBall* ball2 = new CollisionBall(vcl::vec3({ 0,1,2 }), 1.0f);
	CollisionStructure col1;
	CollisionStructure col2;

	for (int i = 1; i < 20; i++) {
		col1.primitive = new CollisionBall(vcl::vec3({ 0,0,0 }), 5.0f / i);
		col1.subStructures.push_back(col1);
	}

	for (int i = 1; i < 20; i++) {
		col2.primitive = new CollisionBall(vcl::vec3({ 0,0,0.5 }), 5.0f / i);
		col2.subStructures.push_back(col2);
	}

	int max_calls = 1000;

	return col1.intersects(col2, max_calls);*/
	return true;
}

bool test_collision_2() {
	// Testing collision
	/*CollisionBall* ball = new CollisionBall(vcl::vec3({ 0,0,1 }), 0.1f);
	CollisionPlane* plane = new CollisionPlane(vcl::vec3({ 0,0,0 }), vcl::vec3({ 1,1,1 }));
	CollisionStructure col1;
	CollisionStructure col2;

	for (int i = 1; i <= 20; i++) {
		col1.primitive = new CollisionBall(vcl::vec3({ 0.3f,0.3f,0.3f }), 5.0f / i);
		col1.subStructures.push_back(col1);
	}

	//col1.primitive = ball;
	col2.primitive = plane;

	int max_calls = 1000;

	return col1.intersects(col2, max_calls);*/
	return true;

}

bool test_collision_3() {
	// Testing collision
	CollisionPlane* plane1 = new CollisionPlane(vcl::vec3({ 1,1,1 }), vcl::vec3({ 0,1,1 }));
	CollisionPlane* plane2 = new CollisionPlane(vcl::vec3({ 0,0,0 }), vcl::vec3({ 1,1,1 }));
	CollisionStructure col1;
	CollisionStructure col2;


	col1.primitive = plane1;
	col2.primitive = plane2;

	int max_calls = 1000;

	return col1.intersects(&col2, max_calls);

}

bool test_collision_4() {
	// Testing collision
	CollisionBall* ball = new CollisionBall(vcl::vec3({ 0,0,0 }), 5.0f);
	CollisionCuboid* cuboid = new CollisionCuboid(vcl::vec3({ 0,0,0 }),
									vcl::vec3({ 1,1,1 }), vcl::vec3({ -1,1,1 }), vcl::vec3({ -1,-1,1 }));
	CollisionStructure col1;
	CollisionStructure col2;

	col1.primitive = ball;
	col2.primitive = cuboid;

	int max_calls = 1000;

	return col1.intersects(&col2, max_calls);
}

bool test_collision_5() {
	CollisionPlane* plane = new CollisionPlane(vcl::vec3({ 0.5,0.5,0 }), vcl::vec3({1,1,1}));
	CollisionCuboid* cuboid = new CollisionCuboid(vcl::vec3({ 0,0,0 }),
		vcl::vec3({ 1,1,2 }), vcl::vec3({ -1,1,1 }), vcl::vec3({ -1,-1,1 }));
	CollisionStructure col1;
	CollisionStructure col2;

	col1.primitive = plane;
	col2.primitive = cuboid;

	int max_calls = 1000;

	return col1.intersects(&col2, max_calls);
}

bool test_collision_6() {
	CollisionCuboid* cuboid1 = new CollisionCuboid(vcl::vec3({ 0.5,0.5,0 }),
		vcl::vec3({ 1,1,2 }), vcl::vec3({ -1,1,1 }), vcl::vec3({ -1,-1,1 }));
	CollisionCuboid* cuboid2 = new CollisionCuboid(vcl::vec3({ 0,0,0 }),
		vcl::vec3({ 1,1,2 }), vcl::vec3({ -1,1,1 }), vcl::vec3({ -1,-1,1 }));
	CollisionStructure col1;
	CollisionStructure col2;

	col1.primitive = cuboid1;
	col2.primitive = cuboid2;

	int max_calls = 1000;

	return col1.intersects(&col2, max_calls);
}

bool plane_intersects_line(vcl::vec3& p1, vcl::vec3& p2, vcl::vec3& normal, vcl::vec3& plane_pos) {
	float side_0 = vcl::dot(p1 - plane_pos, normal);
	float side_1 = vcl::dot(p2 - plane_pos, normal);
	return side_0 * side_1 <= 0.0f;
}

bool point_inside_segment(float p) {
	return p >= 0.0f && p <= 1.0f;
}

bool point_inside_cube(vcl::vec3& new_p) {
	// 
	return	point_inside_segment(new_p.x) &&
			point_inside_segment(new_p.y) &&
			point_inside_segment(new_p.z);
}

bool ball_intersects_ball(CollisionBall* ball1, CollisionBall* ball2) {

	return norm(ball1->pos() - ball2->pos()) < ball1->r + ball2->r;
}

bool ball_intersects_plane(CollisionBall* ball, CollisionPlane* plane) {
	return plane_intersects_line(	ball->pos() + ball->r * plane->normal,
									ball->pos() - ball->r * plane->normal,
									plane->normal, plane->pos());
}

bool plane_intersects_plane(CollisionPlane* plane1, CollisionPlane* plane2) {
	return norm(vcl::cross(plane1->normal, plane2->normal)) > PLANE_INTERSECTION_EPSILON;
}

float line_eq(float t, float P, float Q) {
	return (1 - t) * P + t * Q;
}

float reverse_line_eq(float R, float P, float Q) {
	return (R - P) / (P - Q);
}

bool line_intersects_cuboid(vcl::vec3 p, vcl::vec3 q, CollisionCuboid* cuboid) {
	vcl::mat3 mat({ cuboid->u_x, cuboid->u_y, cuboid->u_z });
	vcl::mat3 inv_mat(inverse_matrix(mat));
	vcl::vec3 new_p(inv_mat * (p - cuboid->pos()));
	vcl::vec3 new_q(inv_mat * (q - cuboid->pos()));

	// If inside/outisde situation
	if (point_inside_cube(new_p)) {
		return !point_inside_cube(new_q);
	}


	// Now determining p position
	// R_t = (1-t)P + tQ : line eq

	// Example : R_t(x) = 1 : t such that we are on +x hyperplane 
	// Then looking if intersection with +x hyperplane is on face with other coordinates

	for (int i = 0; i < 3; i++) {
		if (new_p[i] == new_q[i]) continue; // ATTENTION !!
		if (new_p[i] > 1.0f) {
			float t = reverse_line_eq(1.0f, new_p[i], new_q[i]);
			return point_inside_segment(line_eq(t, new_p[(i + 1) % 3], new_q[(i + 1) % 3]))
				&& point_inside_segment(line_eq(t, new_p[(i + 2) % 3], new_q[(i + 2) % 3]));
		}
		else { // < 0.0f
			float t = reverse_line_eq(0.0f, new_p[i], new_q[i]);
			return point_inside_segment(line_eq(t, new_p[(i + 1) % 3], new_q[(i + 1) % 3]))
				&& point_inside_segment(line_eq(t, new_p[(i + 2) % 3], new_q[(i + 2) % 3]));
		}
	}
}



bool plane_intersects_cuboid(CollisionPlane* plane, CollisionCuboid* cuboid) {
	// on teste les 4 diagonales du cuboid
	vcl::vec3& x = cuboid->u_x;
	vcl::vec3& y = cuboid->u_y;
	vcl::vec3& z = cuboid->u_z;

	vcl::vec3 p000(cuboid->pos());
	vcl::vec3 p111(p000 + x + y + z);

	vcl::vec3 p100(p000 + x);
	vcl::vec3 p011(p000 + y + z);

	vcl::vec3 p110(p000 + x + y);
	vcl::vec3 p001(p000 + z);

	vcl::vec3 p010(p000 + y);
	vcl::vec3 p101(p000 + x + z);

	// now testing intersection
	if (plane_intersects_line(p000, p111, plane->normal, plane->pos())) return true;
	if (plane_intersects_line(p100, p011, plane->normal, plane->pos())) return true;
	if (plane_intersects_line(p110, p001, plane->normal, plane->pos())) return true;
	if (plane_intersects_line(p010, p101, plane->normal, plane->pos())) return true;
	return false;
}

bool cuboid_intersects_cuboid(CollisionCuboid* cuboid1, CollisionCuboid* cuboid2) {

	// on teste les 4 diagonales de chaque cuboid
	vcl::vec3& x = cuboid1->u_x;
	vcl::vec3& y = cuboid1->u_y;
	vcl::vec3& z = cuboid1->u_z;

	vcl::vec3 p000(cuboid1->pos());
	vcl::vec3 p111(p000 + x + y + z);

	vcl::vec3 p100(p000 + x);
	vcl::vec3 p011(p000 + y + z);

	vcl::vec3 p110(p000 + x + y);
	vcl::vec3 p001(p000 + z);

	vcl::vec3 p010(p000 + y);
	vcl::vec3 p101(p000 + x + z);

	// now testing intersection
	if (line_intersects_cuboid(p000, p111, cuboid2)) return true;
	if (line_intersects_cuboid(p100, p011, cuboid2)) return true;
	if (line_intersects_cuboid(p110, p001, cuboid2)) return true;
	if (line_intersects_cuboid(p010, p101, cuboid2)) return true;

	// other cuboid

	x = cuboid2->u_x;
	y = cuboid2->u_y;
	z = cuboid2->u_z;

	p000 = (cuboid2->pos());
	p111 = (p000 + x + y + z);

	p100 = (p000 + x);
	p011 = (p000 + y + z);

	p110 = (p000 + x + y);
	p001 = (p000 + z);

	p010 = (p000 + y);
	p101 = (p000 + x + z);

	if (line_intersects_cuboid(p000, p111, cuboid1)) return true;
	if (line_intersects_cuboid(p100, p011, cuboid1)) return true;
	if (line_intersects_cuboid(p110, p001, cuboid1)) return true;
	if (line_intersects_cuboid(p010, p101, cuboid1)) return true;

	return false;

}

bool ball_intersects_cuboid(CollisionBall* ball, CollisionCuboid* cuboid) {
	// Now calculating each relevant point on ball :
	// We must test 3 diagonals, following cuboid axis
	/*vcl::vec3 P_u_x(ball->pos() + ball->r * cuboid->u_x / norm(cuboid->u_x));
	vcl::vec3 Q_u_x(ball->pos() - ball->r * cuboid->u_x / norm(cuboid->u_x));

	vcl::vec3 P_u_y(ball->pos() + ball->r * cuboid->u_y / norm(cuboid->u_y));
	vcl::vec3 Q_u_y(ball->pos() - ball->r * cuboid->u_y / norm(cuboid->u_y));

	vcl::vec3 P_u_z(ball->pos() + ball->r * cuboid->u_z / norm(cuboid->u_z));
	vcl::vec3 Q_u_z(ball->pos() - ball->r * cuboid->u_z / norm(cuboid->u_z));

	if (line_intersects_cuboid(P_u_x, Q_u_x, cuboid)) return true;
	if (line_intersects_cuboid(P_u_y, Q_u_y, cuboid)) return true;
	if (line_intersects_cuboid(P_u_z, Q_u_z, cuboid)) return true;
	return false;*/

	// We just check with cuboid extremities
	/*vcl::vec3& x = cuboid->u_x;
	vcl::vec3& y = cuboid->u_y;
	vcl::vec3& z = cuboid->u_z;

	vcl::vec3 p000(cuboid->pos());
	vcl::vec3 p111(p000 + x + y + z);

	vcl::vec3 p100(p000 + x);
	vcl::vec3 p011(p000 + y + z);

	vcl::vec3 p110(p000 + x + y);
	vcl::vec3 p001(p000 + z);

	vcl::vec3 p010(p000 + y);
	vcl::vec3 p101(p000 + x + z);

	for (auto& point : { p000 , p111, p100, p011, p110, p001, p010, p101 }) {
		if (norm(point - ball->pos()) < ball->r) return true;
	}

	return false;*/

	// too hard, we are just approximating by cuboid
	// We must look for closest point in cube. first if inside cube
	/*vcl::mat3 mat({ cuboid->u_x, cuboid->u_y, cuboid->u_z });
	vcl::mat3 inv_mat(inverse_matrix(mat));
	vcl::vec3 pos = ball->pos() - cuboid->pos();
	vcl::vec3 new_pos = inv_mat * pos;
	if(point_inside_cube(inv_mat*(ball->pos() - cuboid->pos()))) return true;

	 else, calculation in inv space
	 We calculate intersection of spheroid and cube
	if (pos)*/
	vcl::vec3 pos = ball->pos();
	CollisionCuboid cuboid_approx(pos - vcl::vec3({ 1,1,1 }) * ball->r, vcl::vec3({ 1,0,0 }) * ball->r, vcl::vec3({ 0,1,0 }) * ball->r, vcl::vec3({ 0,0,1 }) * ball->r);
	return cuboid_intersects_cuboid(&cuboid_approx, cuboid);

}

CollisionPrimitive::CollisionPrimitive(PrimitiveType _type, vcl::vec3 _pos)
	:	type(_type)
	,	local_pos(_pos)
	,	global_pos({ 0,0,0 })
{

}

vcl::vec3 CollisionPrimitive::pos() const {
	return global_pos + local_pos;
}


bool CollisionPrimitive::intersects(CollisionPrimitive* prim) {
	switch (this->type) {
	case PrimitiveType::Ball : {
		switch (prim->type) {
		case PrimitiveType::Ball :
			return ball_intersects_ball(reinterpret_cast<CollisionBall*>(this), 
										reinterpret_cast<CollisionBall*>(prim));
		case PrimitiveType::Plane :
			return ball_intersects_plane(reinterpret_cast<CollisionBall*>(this),
										reinterpret_cast<CollisionPlane*>(prim));
		case PrimitiveType::Cuboid:
			return ball_intersects_cuboid(reinterpret_cast<CollisionBall*>(this),
										reinterpret_cast<CollisionCuboid*>(prim));
		}
	}
	case PrimitiveType::Plane: {
		switch (prim->type) {
		case PrimitiveType::Ball:
			return ball_intersects_plane(reinterpret_cast<CollisionBall*>(prim),
										reinterpret_cast<CollisionPlane*>(this));
		case PrimitiveType::Plane:
			return plane_intersects_plane(reinterpret_cast<CollisionPlane*>(this),
										reinterpret_cast<CollisionPlane*>(prim));
		case PrimitiveType::Cuboid:
			return plane_intersects_cuboid(reinterpret_cast<CollisionPlane*>(this),
										reinterpret_cast<CollisionCuboid*>(prim));
		}
	}
	case PrimitiveType::Cuboid: {
		switch (prim->type) {
		case PrimitiveType::Ball:
			return ball_intersects_cuboid(reinterpret_cast<CollisionBall*>(prim),
										reinterpret_cast<CollisionCuboid*>(this));
		case PrimitiveType::Plane:
			return plane_intersects_cuboid(reinterpret_cast<CollisionPlane*>(prim),
										reinterpret_cast<CollisionCuboid*>(this));
		case PrimitiveType::Cuboid:
			return cuboid_intersects_cuboid(reinterpret_cast<CollisionCuboid*>(prim),
										reinterpret_cast<CollisionCuboid*>(this));
		}
	}
	}
}


CollisionBall::CollisionBall(vcl::vec3 pos, float radius)
	:	CollisionPrimitive(PrimitiveType::Ball, pos)
	,	r(radius)
{
	assert_vcl(r >= 0, "Error : radius used to define ball is negative");
}


CollisionPlane::CollisionPlane(vcl::vec3 pos, vcl::vec3 _normal) 
	: CollisionPrimitive(PrimitiveType::Plane, pos)
	, normal(_normal)
{
	// verify normal is not null
	float nor = norm(_normal);
	assert_vcl(nor > VECTOR_MINIMAL_EPSILON, "Error : normal used to define plane is null");
	normal /= nor;
}

CollisionCuboid::CollisionCuboid(vcl::vec3 pos, vcl::vec3 _u_x, vcl::vec3 _u_y, vcl::vec3 _u_z)
	: CollisionPrimitive(PrimitiveType::Cuboid, pos)
	, u_x(_u_x) 
	, u_y(_u_y)
	, u_z(_u_z)
{
	// verify that we have direct basis and no three vector are null
	float n_x = norm(u_x);
	float n_y = norm(u_y);
	float n_z = norm(u_z);
	assert_vcl( std::min(n_x, std::min(n_y, n_z)) > VECTOR_MINIMAL_EPSILON, "Error : trying to use too small vectors for cuboid");

	// dot product cannot be null (if rectangle or line)
	assert_vcl(vcl::dot(vcl::cross(u_x, u_y), u_z) > 0.0f, "Error : trying to use indirect basis for cuboid");
}

CollisionStructure::CollisionStructure()
	:	primitive(nullptr)
{

}

CollisionStructure::~CollisionStructure()
{
	if (primitive) delete primitive;
	for (auto& sub : subStructures) delete sub;
}

void CollisionStructure::operator=(CollisionStructure& cs) {
	primitive = cs.primitive;
	subStructures = cs.subStructures;
}

void CollisionStructure::move_first_primitive(vcl::vec3& v) {
	primitive->global_pos = v;
}


CollisionPrimitive* CollisionStructure::intersects(CollisionStructure* structure, int& max_sub_calls) {
	// If too much time, we consider there is no collision
	if (!max_sub_calls) return nullptr;

	// primitive global_pos is currently set right

	// First testing against parent structure given on input
	if (primitive->intersects(structure->primitive)) {
		// we continue on children if they exist
		if (structure->subStructures.size() == 0) {
			// we are done
			return primitive;
		}
		else {
			// Now launching the other's children on this, so we get ping pong effect
			// that makes a hierarchical parcours, as intended
			// We must be careful to update the children global_pos from here
			for (auto& substructre : structure->subStructures) {
				substructre->primitive->global_pos = structure->primitive->pos();
				CollisionPrimitive* prim = substructre->intersects(this, --max_sub_calls);
				if (prim) return substructre->primitive; // we return the other 
			}
		}
	}
	return false;
}




/*
bool CollisionBall::intersects(CollisionPrimitive* prim) {
	switch (prim->type) {
		case PrimitiveType::Ball:
		{
			CollisionBall* ball = dynamic_cast<CollisionBall*>(prim);
			return ball_intersects_ball(*this, *ball);
		}
		case PrimitiveType::Plane :
		{
			CollisionPlane* plane = dynamic_cast<CollisionPlane*>(prim);
			return ball_intersects_plane(*this, *plane);
		}
		case PrimitiveType::Cuboid:
		{
			CollisionCuboid* cuboid = dynamic_cast<CollisionCuboid*>(prim);
			return ball_intersects_cuboid(*this, *cuboid);
		}
	}

	return false;
}
*/

/*
bool CollisionPlane::intersects(CollisionPrimitive* prim) {
	switch (prim->type) {
		case PrimitiveType::Ball:
		{
			CollisionBall* ball = dynamic_cast<CollisionBall*>(prim);
			return ball_intersects_plane(*ball, *this);
		}
		case PrimitiveType::Plane:
		{
			CollisionPlane* plane = dynamic_cast<CollisionPlane*>(prim);
			return plane_intersects_plane(*plane, *this);
		}
		case PrimitiveType::Cuboid:
		{
			CollisionCuboid* cuboid = dynamic_cast<CollisionCuboid*>(prim);
			return plane_intersects_cuboid(*this, *cuboid);
		}
	}
}
*/

/*
bool CollisionCuboid::intersects(CollisionPrimitive* prim) {
	switch (prim->type) {
		case PrimitiveType::Ball:
		{
			CollisionBall* ball = dynamic_cast<CollisionBall*>(prim);
			return ball_intersects_cuboid(*ball, *this);
		}
		case PrimitiveType::Plane:
		{
			CollisionPlane* plane = dynamic_cast<CollisionPlane*>(prim);
			return plane_intersects_cuboid(*plane, *this);
		}
		case PrimitiveType::Cuboid:
		{
			CollisionCuboid* cuboid = dynamic_cast<CollisionCuboid*>(prim);
			return cuboid_intersects_cuboid(*cuboid, *this);
		}
	}
}
*/


// cuboid cuboid
/*vcl::mat3 mat({ cuboid2->u_x, cuboid2->u_y, cuboid2->u_z });
	vcl::mat3 inv_mat(inverse_matrix(mat));

	// getting cuboid1 vector in space where cuboid2 is cube
	vcl::vec3& new_x = inv_mat * cuboid1->u_x;
	vcl::vec3& new_y = inv_mat * cuboid1->u_y;
	vcl::vec3& new_z = inv_mat * cuboid1->u_z;

	// Now getting each point of cuboid1 in new space
	vcl::vec3 p000(inv_mat * (cuboid1->pos - cuboid2->pos));
	vcl::vec3 p111(p000 + new_x + new_y + new_z);

	vcl::vec3 p100(p000 + new_x);
	vcl::vec3 p011(p000 + new_y + new_z);

	vcl::vec3 p110(p000 + new_x + new_y);
	vcl::vec3 p001(p000 + new_z);

	vcl::vec3 p010(p000 + new_y);
	vcl::vec3 p101(p000 + new_x + new_z);

	// and testing each points
	if (point_inside_cube(p000)) return true;
	if (point_inside_cube(p001)) return true;
	if (point_inside_cube(p010)) return true;
	if (point_inside_cube(p011)) return true;
	if (point_inside_cube(p100)) return true;
	if (point_inside_cube(p101)) return true;
	if (point_inside_cube(p110)) return true;
	if (point_inside_cube(p111)) return true;
	return false;

	// pfiou*/

//ball cuboid

/*
	vcl::vec3 new_x(inv_mat * cuboid->u_x);
	if (point_inside_cube(new_p + new_x)) return true;
	if (point_inside_cube(new_p - new_x)) return true;
	vcl::vec3 new_y(inv_mat * cuboid->u_y);
	if (point_inside_cube(new_p + new_y)) return true;
	if (point_inside_cube(new_p - new_y)) return true;
	vcl::vec3 new_z(inv_mat * cuboid->u_z);
	if (point_inside_cube(new_p + new_z)) return true;
	if (point_inside_cube(new_p - new_z)) return true;
	return false;
*/