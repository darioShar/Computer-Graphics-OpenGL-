#pragma once

#include "vcl/vcl.hpp"
#include "Collision.hpp"
#include "DefaultMeshDrawable.hpp"

class Tree : public DefaultMeshDrawable {
public :
	CollisionStructure* structure;

	Tree();
	~Tree();
	void create();
	void move(vcl::vec3 v);
};

namespace vcl
{
	template <typename SCENE>
	void draw(Tree& drawable, SCENE const& scene)
	{
		draw(static_cast<DefaultMeshDrawable>(drawable), scene);
		draw(drawable.structure, scene);
	}
}