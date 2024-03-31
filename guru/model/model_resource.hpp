/**
 * model_resource.hpp
 * ---
 * this file defines the ModelResource class,
 * which contains a series of Meshes that
 * are used to render a loaded 3D object.
 *
 */

#pragma once
#include "mesh.hpp"

namespace gu {
class ModelResource {
protected:
	std::vector<Mesh> _meshes;
	GLenum _face_cull_option = GL_NONE;
	std::filesystem::path _path;

public:
	ModelResource(
		const std::filesystem::path &path, 
		const GLenum &face_cull_option=GL_NONE
	);

	~ModelResource();

	void load(const std::filesystem::path& path);

protected:
	void _process_node(aiNode *node, const aiScene *scene);

public:
	void set_face_cull_option(const GLenum &cull_option);
};
} // namespace gu

/*
Model {
	uses_normal_maps
	uses_specular_maps
	uses_roughness_maps
	uses_displacement_maps
	uses_emissive_maps
}

add texture replacements "blue_shirt.png" -> "green_shirt.png"
*/