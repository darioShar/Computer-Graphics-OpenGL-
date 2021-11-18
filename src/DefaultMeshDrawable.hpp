#pragma once
#include <vcl/vcl.hpp>


/* Default class for mesh drawable creation. We must be carefull and not make any call to opengl
 on constructor in case we want to define our object as global variables, and opengl is uniintialized
 */

class DefaultMeshDrawable {
protected :
	vcl::mesh d_mesh;
	vcl::mesh_drawable d_mesh_drawable;
	void update_mesh_drawable();

public :
	static void setDefaultMeshShader();
	static void setDefaultMeshTexture();

	bool isInitialized();
	void create(vcl::mesh& _mesh);
	void move(vcl::vec3& v);
	void rotate(vcl::rotation& rot);

	void set_texture(const std::string& str);
	vcl::shading_parameters_phong& shading();
	vcl::mesh_drawable& get_drawable();
};

namespace vcl
{
	template <typename SCENE>
	void draw(DefaultMeshDrawable& drawable, SCENE const& scene)
	{
		draw(drawable.get_drawable(), scene);
	}
}