#include "Flamethrower.hpp"

Flamethrower::Flamethrower()
{

}

void Flamethrower::calculate_weapon_rotation(fps_view& head_scene) {
	//d_mesh_drawable.transform.rotate = vcl::rotation(vcl::vec3({ 1,0,0 }), vcl::pi / 2);
	d_mesh_drawable.transform.rotate = head_scene.get_camera()->orientation();
	d_mesh_drawable.transform.rotate *= vcl::rotation(vcl::vec3({ 0,1,0 }), vcl::pi / 2);
}
void Flamethrower::calculate_weapon_translation(fps_view& head_scene) {
	d_mesh_drawable.transform.translate = head_scene.get_camera()->position()
		+ head_scene.get_camera()->front() * 0.4f + head_scene.get_camera()->right() * 0.35f;
	d_mesh_drawable.transform.translate.z -= head_scene.height * 0.5f;
}

void Flamethrower::calculate_weapon_barrel_pos(fps_view& head_scene) {
	barrel_pos = d_mesh_drawable.transform.translate + head_scene.camera.front() * 0.8f + head_scene.camera.up() * 0.3f;
}

void Flamethrower::create() {
	Weapon::create(FLAMETHROWER_FILE);
	set_texture(FLAMETHROWER_TEXTURE);

	d_mesh_drawable.transform.scale *= 0.3f;
}

void Flamethrower::draw_fps(fps_view& head_scene) {
	// translate and rotate weapon
	calculate_weapon_translation(head_scene);
	calculate_weapon_rotation(head_scene);
	// update barrel pos
	calculate_weapon_barrel_pos(head_scene);

	draw(d_mesh_drawable, head_scene);
}

vcl::vec3& Flamethrower::get_barrel_pos() {
	return barrel_pos;
}
