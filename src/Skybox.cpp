#include "Skybox.hpp"

void Skybox::create(const std::string& file, float cube_size) {
	// First define mesh to use. 6 quadrangles.
	// get points
	vcl::vec3 x({ cube_size,0,0 });
	vcl::vec3 y({ 0,cube_size,0 });
	vcl::vec3 z({ 0,0,cube_size });

	vcl::vec3 p000({-cube_size /2.0f, -cube_size / 2.0f , -cube_size / 2.0f });
	vcl::vec3 p111(p000 + x + y + z);

	vcl::vec3 p100(p000 + x);
	vcl::vec3 p011(p000 + y + z);

	vcl::vec3 p110(p000 + x + y);
	vcl::vec3 p001(p000 + z);

	vcl::vec3 p010(p000 + y);
	vcl::vec3 p101(p000 + x + z);

	/*vcl::mesh side_0(vcl::mesh_primitive_quadrangle(p000, p100, p101, p001)); // xz -y
	vcl::mesh side_1(vcl::mesh_primitive_quadrangle(p010, p110, p111, p011)); // xz +y
	vcl::mesh side_2(vcl::mesh_primitive_quadrangle(p000, p010, p110, p100)); // xy -z
	vcl::mesh side_3(vcl::mesh_primitive_quadrangle(p001, p011, p111, p101)); // xy +z
	vcl::mesh side_4(vcl::mesh_primitive_quadrangle(p010, p011, p001, p000)); // zy -x
	vcl::mesh side_5(vcl::mesh_primitive_quadrangle(p110, p111, p101, p100)); //zy + x*/

	vcl::mesh side_0(vcl::mesh_primitive_quadrangle(p100, p000, p001, p101)); 
	vcl::mesh side_1(vcl::mesh_primitive_quadrangle(p000, p010, p011, p001)); 
	vcl::mesh side_2(vcl::mesh_primitive_quadrangle(p010, p110, p111, p011)); 
	vcl::mesh side_3(vcl::mesh_primitive_quadrangle(p110, p100, p101, p111)); 
	vcl::mesh side_4(vcl::mesh_primitive_quadrangle(p100, p110, p010, p000)); 
	vcl::mesh side_5(vcl::mesh_primitive_quadrangle(p001, p011, p111, p101));

	// On colle le patron aux faces
	for (auto& uv : side_0.uv) {
		uv.x /= 4.0f;
		uv.y /= 3.0f;
		uv += vcl::vec2({ 0,1.0f / 3.0f });
	}
	for (auto& uv : side_1.uv) {
		uv.x /= 4.0f;
		uv.y /= 3.0f;
		uv += vcl::vec2({ 0.25f,1.0f / 3.0f });
	}
	for (auto& uv : side_2.uv) {
		uv.x /= 4.0f;
		uv.y /= 3.0f;
		uv += vcl::vec2({ 0.5f,1.0f / 3.0f });
	}
	for (auto& uv : side_3.uv) {
		uv.x /= 4.0f;
		uv.y /= 3.0f;
		uv += vcl::vec2({ 0.75f,1.0f / 3.0f });
	}
	for (auto& uv : side_4.uv) {
		uv.x /= 4.0f;
		uv.y /= 3.0f;
		uv += vcl::vec2({ 0.25f, 0.0f });
	}
	for (auto& uv : side_5.uv) {
		uv.x /= 4.0f;
		uv.y /= 3.0f;
		uv += vcl::vec2({ 0.25f, 2.0f / 3.0f });
	}

	// Construction du cube
	vcl::mesh cube(side_0);
	cube.push_back(side_1);
	cube.push_back(side_2);
	cube.push_back(side_3);
	cube.push_back(side_4);
	cube.push_back(side_5);

	cube.fill_empty_field();

	d_mesh_drawable = vcl::mesh_drawable(cube);

	// On regle les paramètres du shader
	d_mesh_drawable.shading.phong.diffuse = 0.0f;
	d_mesh_drawable.shading.phong.ambient = 1.0f;
	d_mesh_drawable.shading.phong.specular = 0.0f;

	// Maintenant on charge la texture
	set_texture(file);
	// voila
}
