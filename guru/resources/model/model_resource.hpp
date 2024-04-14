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
	std::vector<Mesh> _meshes; // VAOs
	std::vector<std::shared_ptr<Material>> _materials; // materials
	std::vector<size_t> _transparent_mesh_indices;
	std::vector<size_t> _opaque_mesh_indices;
	GLenum _face_cull_option = GL_BACK;
	std::filesystem::path _path; // path to the loaded 3D model
	bool _uses_map[Material::N_MAP_TYPES]; // [i] is true if Mesh loaded map

public:
	// ctor. initializes member variables.
	ModelResource();

	// ctor. loads 3D object from the given <path> 
	// and sets the face-cull option.
	ModelResource(
		const std::filesystem::path &path, 
		const GLenum &face_cull_option = GL_BACK
	);

	// dtor. tells the global MaterialList
	// to delete Materials if no other instance is using them.
	~ModelResource();

	// loads a 3D object from the given <path>.
	void load(const std::filesystem::path& path);

protected:
	// processes a given node and its contained aiMeshes, with each 
	// aiMesh's data being transferred to the object's list of Meshes.
	void _process_node(size_t &n_meshes, aiNode *node, const aiScene *scene);

public:
	// returns true if this ModelResource uses a particular map type.
	inline bool uses_map(uint8_t map_type) const {
		return (map_type < Material::N_MAP_TYPES and _uses_map[map_type]);
	}

	// returns the file path to the 3D object file.
	inline const std::filesystem::path &path() const { return _path; }

	// takes the given <mesh_indices> vector and pushes back 
	// the indices of the Meshes whose Materials 
	// have their local diffuse path set to <search_local_path>.
	void get_mesh_indices_by_path(
		std::vector<size_t> &mesh_indices,
		const std::filesystem::path &search_local_path
	) const;

	// returns the index of the Material in the object's list of Materials
	// whose local diffuse path matches the <search_local_path>.
	size_t get_material_index_by_path(
		const std::filesystem::path &search_local_path
	) const;

	// sets the face culling option to GL_FRONT, GL_BACK, or GL_NONE.
	void set_face_cull_option(const GLenum &cull_option);

	// draws all the meshes of the ModelResource.
	void draw_meshes(
		const std::vector<Material::Override> &material_overrides = (
			std::vector<Material::Override>()
		),
		const std::vector<Mesh::Override> &mesh_overrides = (
			std::vector<Mesh::Override>()
		)
	);

	// draws the transparent meshes of the ModelResource.
	// ---
	// <material_overrides> can be given
	// to override the Material in the ModelResource's list of Materials,
	// which will change the Material for all Meshes using it.
	// ---
	// <mesh_overrides> can be given
	// to change which Material is bound per Mesh.
	void draw_transparent_meshes(
		const std::vector<Material::Override> &material_overrides = (
			std::vector<Material::Override>()
		),
		const std::vector<Mesh::Override> &mesh_overrides = (
			std::vector<Mesh::Override>()
		)
	);

	// draws the opaque meshes of the ModelResource.
	// ---
	// <material_overrides> can be given
	// to override the Material in the ModelResource's list of Materials,
	// which will change the Material for all Meshes using it.
	// ---
	// <mesh_overrides> can be given
	// to change which Material is bound per Mesh.
	void draw_opaque_meshes(
		const std::vector<Material::Override> &material_overrides = (
			std::vector<Material::Override>()
		),
		const std::vector<Mesh::Override> &mesh_overrides = (
			std::vector<Mesh::Override>()
		)
	);

protected:
	// draws the object's list of Meshes, whose <mesh_indices> are specified.
	// ---
	// <material_overrides> can be given
	// to override the Material in the ModelResource's list of Materials,
	// which will change the Material for all Meshes using it.
	// ---
	// <mesh_overrides> can be given
	// to change which Material is bound per Mesh.
	// ---
	// if <use_face_culling> is true, 
	// then the ModelResource's face-culling option will be applied.
	void _draw_mesh_by_indices(
		const std::vector<Material::Override> &material_overrides,
		const std::vector<Mesh::Override> &mesh_overrides,
		const std::vector<size_t> &mesh_indices,
		const bool use_face_culling
	);
};
} // namespace gu