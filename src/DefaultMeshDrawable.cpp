#include "DefaultMeshDrawable.hpp"

void DefaultMeshDrawable::setDefaultMeshShader() {
    GLuint const shader_mesh = vcl::opengl_create_shader_program(vcl::opengl_shader_preset("mesh_vertex"),
        vcl::opengl_shader_preset("mesh_fragment"));
    vcl::mesh_drawable::default_shader = shader_mesh;

}

void DefaultMeshDrawable::setDefaultMeshTexture() {
    GLuint const texture_white = vcl::opengl_texture_to_gpu(vcl::image_raw{ 1,1,vcl::image_color_type::rgba,{255,255,255,255} });
    vcl::mesh_drawable::default_texture = texture_white;
}

// See if there is mesh to render
bool DefaultMeshDrawable::isInitialized() {
    return d_mesh_drawable.number_triangles != 0;
}


void DefaultMeshDrawable::create(vcl::mesh& _mesh) {
    d_mesh = _mesh;
    d_mesh_drawable = vcl::mesh_drawable(d_mesh);
}

void DefaultMeshDrawable::move(vcl::vec3& v) {
    d_mesh_drawable.transform.translate = v;
}

void DefaultMeshDrawable::rotate(vcl::rotation& rot) {
    d_mesh_drawable.transform.rotate = rot;

}


void DefaultMeshDrawable::update_mesh_drawable() {
    // Update step: Allows to update a mesh_drawable without creating a new one
    d_mesh_drawable.update_position(d_mesh.position);
    d_mesh_drawable.update_normal(d_mesh.normal);
    d_mesh_drawable.update_color(d_mesh.color);
}

void DefaultMeshDrawable::set_texture(const std::string& str) {
    vcl::image_raw im = vcl::image_load_png(str); // texture
    GLuint texture_image_id = vcl::opengl_texture_to_gpu(im, GL_REPEAT, GL_REPEAT);
    d_mesh_drawable.texture = texture_image_id;
}

vcl::shading_parameters_phong& DefaultMeshDrawable::shading() {
    return d_mesh_drawable.shading;
}

vcl::mesh_drawable& DefaultMeshDrawable::get_drawable() {
	return d_mesh_drawable;
}