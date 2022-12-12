#pragma once
#include "vcl/vcl.hpp"
#include "DefaultMeshDrawable.hpp"

class Weapon : public DefaultMeshDrawable {

public :
	Weapon();

	void create(const std::string& obj_file);
};