#pragma once
#include "Weapon.hpp"
#include "scene.hpp"

#define FLAMETHROWER_FILE "assets/obj/source/Flamethrowe2.obj"
#define FLAMETHROWER_TEXTURE "assets/obj/textures/Metallic.png"


class Flamethrower : public Weapon {
	void calculate_weapon_rotation(fps_view& head_scene);
	void calculate_weapon_translation(fps_view& head_scene);
	void calculate_weapon_barrel_pos(fps_view& head_scene);

	vcl::vec3 barrel_pos;

public :
	Flamethrower();
	void create();
	void draw_fps(fps_view& head_scene);
	vcl::vec3& get_barrel_pos();
};