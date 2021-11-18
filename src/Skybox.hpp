#pragma once

#include "DefaultMeshDrawable.hpp"

class Skybox : public DefaultMeshDrawable {

public :
	
	void create(const std::string& file, float cube_size);
};