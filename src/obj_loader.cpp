#include "obj_loader.h"
#include "third_party/src/obj_loader/OBJ_Loader.h"


void load_mesh(vcl::mesh& mesh_to_load, const std::string filePath) {
	objl::Loader loader;
	if (!loader.LoadFile(filePath)) {
		std::cerr << "Error loading .obj file : could not load file located at "
			<< filePath << std::endl;
		abort();
	}

	for (objl::Vertex& vertex : loader.LoadedVertices) {
		mesh_to_load.position.push_back(
			vcl::vec3(vertex.Position.X, vertex.Position.Y, vertex.Position.Z));
		vcl::vec3 normal(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z);
		normal /= norm(normal);
		mesh_to_load.normal.push_back(normal);
		mesh_to_load.uv.push_back(
			vcl::vec2(vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y));
	}
	
	for (int i = 0; i < loader.LoadedIndices.size(); i += 3) {
		mesh_to_load.connectivity.push_back(
			vcl::uint3(loader.LoadedIndices[i], 
				loader.LoadedIndices[i + 1], 
				loader.LoadedIndices[i + 2])
		);
	}

	mesh_to_load.fill_empty_field();
}