#include "Weapon.hpp"
#include "obj_loader.h"

Weapon::Weapon() 
{
	
}

void Weapon::create(const std::string& obj_file) {
	load_mesh(d_mesh, obj_file);
	d_mesh_drawable = vcl::mesh_drawable(d_mesh);
}
