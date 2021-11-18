#pragma once

#include <vcl/vcl.hpp>
#include "DefaultMeshDrawable.hpp"
#include "utils.hpp"

#define PLANE_INTERSECTION_EPSILON 1e-2
#define VECTOR_MINIMAL_EPSILON 1e-4


enum class PrimitiveType {
	Ball,
	Plane,
	Cuboid
};

struct CollisionPrimitive {
	PrimitiveType type;
	vcl::vec3 global_pos;
	vcl::vec3 local_pos;
	// global_pos will be used from top primitive to down primitive, so ultimately only important
	// to specify top primitive global_pos, rest will be deduced from local_pos
	CollisionPrimitive(PrimitiveType _type, vcl::vec3 _pos);
	vcl::vec3 pos() const;
	bool intersects(CollisionPrimitive*);
};

struct CollisionBall : public CollisionPrimitive {
	float r;
	CollisionBall(vcl::vec3 pos, float radius);
};

struct CollisionPlane : public CollisionPrimitive {
	vcl::vec3 normal;
	CollisionPlane(vcl::vec3 pos, vcl::vec3 _normal);
};

struct CollisionCuboid : public CollisionPrimitive {
	vcl::vec3 u_x;
	vcl::vec3 u_y;
	vcl::vec3 u_z;

	CollisionCuboid(vcl::vec3 pos, vcl::vec3 _u_x, vcl::vec3 _u_y, vcl::vec3 _u_z);
};


class CollisionStructure : public DefaultMeshDrawable {
public :
	CollisionPrimitive* primitive;
	std::vector<CollisionStructure*> subStructures;

	static float alpha;
	static bool show_hitbox;

	CollisionStructure();
	~CollisionStructure();

	void move_first_primitive(vcl::vec3& v);
	void operator=(CollisionStructure&);
	CollisionPrimitive* intersects(CollisionStructure* structure, int& max_sub_calls);

private : //  we don't want = operator
	/*void operator=(CollisionStructure) {}
	void operator=(CollisionStructure&) {}
	void operator=(CollisionStructure*) {}*/

};

namespace vcl
{
	template <typename SCENE>
	void draw(CollisionStructure* drawable, SCENE const& scene)
	{
		if (!CollisionStructure::show_hitbox) return;

		// to draw transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(false);

		// and moving the mesh drawable
		drawable->get_drawable().transform.translate = drawable->primitive->pos();
		if (drawable->isInitialized()) {
			drawable->get_drawable().shading.color = vcl::vec3({ 1,0,0 });
			drawable->get_drawable().shading.alpha = CollisionStructure::alpha;
			draw(*static_cast<DefaultMeshDrawable*>(drawable), scene);
		}
		// don't forget to move sub structure global pos
		for (CollisionStructure* substruct : drawable->subStructures) {
			substruct->primitive->global_pos = drawable->primitive->pos();
			draw(substruct, scene);
		}

		glDepthMask(true);
		glDisable(GL_BLEND);
	}
}

bool ball_intersects_ball(CollisionBall* ball1, CollisionBall* ball2);
bool ball_intersects_plane(CollisionBall* ball, CollisionPlane* plane);
bool plane_intersects_plane(CollisionPlane* plane1, CollisionPlane* plane2);




/* Working with these :
CollisionStructure::CollisionStructure()
	:	primitive(nullptr)
{

}

void CollisionStructure::operator=(CollisionStructure& cs) {
	primitive = cs.primitive;
	subStructures = cs.subStructures;
}

*/

// How many tests ? 2 choose n + n
bool test_collision_1();
bool test_collision_2();
bool test_collision_3();
bool test_collision_4();
bool test_collision_5();
bool test_collision_6();