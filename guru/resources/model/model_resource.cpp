#include "model_resource.hpp"
#include <iostream>
#include <set>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../material/material_list.hpp"

static auto &material_list = gu::res::MaterialList::material_list;

namespace gu {
ModelResource::ModelResource() {
	// initializes member array.
	_uses_map[0] = true;
	for (uint8_t i = 1; i < Material::N_MAP_TYPES; ++i)
		_uses_map[i] = false;
}

ModelResource::ModelResource(
	const std::filesystem::path& path, const GLenum &face_cull_option
) {
	// initializes member array.
	_uses_map[0] = true;
	for (uint8_t i = 1; i < Material::N_MAP_TYPES; ++i)
		_uses_map[i] = false;
	
	// sets up 3D model.
	set_face_cull_option(face_cull_option);
	load(path);
}

ModelResource::~ModelResource() {
	// creates a set of all unique Material load paths.
	std::set<std::filesystem::path> material_paths;
	for (const auto& material : _materials)
		material_paths.insert(material->path());

	// destroys Meshes, which also destroys their shared pointers to Materials
	// and now the ModelResource can properly delete the Materials used.
	std::vector<Mesh>().swap(_meshes);
	for (const auto &material_path : material_paths)
		material_list.delete_entry(material_path);
}

// sets the given <n_meshes> to the number of meshes contained
// by the given aiNode <node> through recursion.
static void count_meshes(size_t &n_meshes, aiNode *node) {
	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		++n_meshes;
	for (uint32_t i = 0; i < node->mNumChildren; ++i)
		count_meshes(n_meshes, node->mChildren[i]);
}

void ModelResource::load(const std::filesystem::path &path) {
	if (not std::filesystem::exists(path)) {
		std::cerr << path << " could not be found." << std::endl;
		return;
	}

	// uses the assimp library to load the 3D object file.
	Assimp::Importer importer;
	importer.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", 90);
	const aiScene* scene = importer.ReadFile(
		path.string(),
		  aiProcess_Triangulate 
		| aiProcess_GenSmoothNormals
		| aiProcess_FlipUVs
		#if not defined(GURU_DISABLE_TANGENT_SPACE)
		| aiProcess_CalcTangentSpace
		#endif	
	);
	if (
		not scene 
		or scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE 
		or not scene->mRootNode
	) {
		std::cerr 
			<< "Error with Model::load:\n" 
			<< importer.GetErrorString()
			<< std::endl;
		return;
	}
	_path = path;

	// determines the amount of meshes that will be used.
	// this is pre-allocated to prevent Mesh dtors from being called
	// by the vector having to reallocate itself.
	size_t n_meshes = 0;
	count_meshes(n_meshes, scene->mRootNode);
	_meshes.reserve(n_meshes);

	// fills the pre-allocated <_meshes> vector with the file's data.
	n_meshes = 0;
	_process_node(n_meshes, scene->mRootNode, scene);
	for (const auto &material : _materials) {
		for (uint8_t j = 1; j < Material::N_MAP_TYPES; ++j) {
			if (material->uses_map(j)) {
				_uses_map[j] = true;
				break;
			}
		}
	}
}

// returns the index of the Material in the given <materials> vector
// whose diffuse texture path matches the given <diffuse_path>.
static size_t find_material_index(
	const std::filesystem::path &diffuse_path,
	const std::vector<std::shared_ptr<Material>> &materials
) {
	size_t i = materials.size();
	for (i = 0; i < materials.size(); ++i)
		if (materials[i]->path() == diffuse_path)
			break;
	return i;
}

// returns the index of the given <materials> vector that indicates
// which Material shared pointer for the Mesh to use for drawing.
static size_t load_material(
	std::vector<std::shared_ptr<Material>> &materials,
	const aiMaterial *const ai_material,
	const std::filesystem::path &model_directory
) {
	if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) == 0)
		return materials.size();

	// gets the diffuse image path.
	aiString ai_diffuse;
	ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_diffuse);
	std::filesystem::path diffuse_path = model_directory / ai_diffuse.C_Str();

	// determines if this Material already exists in the ModelResource's <_materials>.
	size_t existing_mat_index = find_material_index(diffuse_path, materials);
	if (existing_mat_index < materials.size())
		return existing_mat_index;

	// determines if this Material has already been loaded
	// based off the diffuse texture path. 
	// if so, a shared pointer to the existing one is used.
	std::shared_ptr<Material> existing_material = (
		material_list.find_existing(diffuse_path)
	);
	if (existing_material) {
		materials.push_back(existing_material);
		return materials.size() - 1;
	}

	// gets local image paths for each kind of map from the loaded aiMaterial.
	std::filesystem::path paths[Material::N_MAP_TYPES];
	paths[0] = diffuse_path;
	aiString ai_image_names[Material::N_MAP_TYPES];
	#if not defined(GURU_DISABLE_TANGENT_SPACE)
	ai_material->GetTexture(
		aiTextureType_NORMALS,
		0,
		&ai_image_names[Material::MAP_TYPE::NORMAL]
	);
	#endif

	ai_material->GetTexture(
		aiTextureType_SPECULAR,
		0,
		&ai_image_names[Material::MAP_TYPE::SPECULAR]
	);

	ai_material->GetTexture(
		aiTextureType_SHININESS,
		0,
		&ai_image_names[Material::MAP_TYPE::ROUGHNESS]
	);

	#if not defined(GURU_DISABLE_TANGENT_SPACE)
	ai_material->GetTexture(
		aiTextureType_DISPLACEMENT,
		0,
		&ai_image_names[Material::MAP_TYPE::DISPLACEMENT]
	);
	#endif

	ai_material->GetTexture(
		aiTextureType_EMISSIVE,
		0,
		&ai_image_names[Material::MAP_TYPE::EMISSIVE]
	);

	// creates global image file paths.
	for (uint8_t i = 1; i < Material::N_MAP_TYPES; ++i) {
		if (ai_image_names[i].length > 0)
			paths[i] = model_directory / ai_image_names[i].C_Str();
	}

	// loads the Material's textures.
	materials.push_back(material_list.create_and_load(paths));
	return materials.size() - 1;
}

void ModelResource::_process_node(size_t &n_meshes, aiNode *node, const aiScene *scene) {
	std::filesystem::path directory = _path.parent_path();

	// loads the contained meshes to the <_meshes> vector.
	for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
		aiMesh *const ai_mesh = scene->mMeshes[node->mMeshes[i]];
		aiMaterial *const ai_mat = scene->mMaterials[ai_mesh->mMaterialIndex];

		// loads material.
		size_t material_index = load_material(
			_materials,
			scene->mMaterials[ai_mesh->mMaterialIndex],
			directory
		);

		// loads the Mesh's geometry and gives it its <_material_index>,
		// then the Mesh's index is organized.
		_meshes.emplace_back();
		_meshes[n_meshes].load(ai_mesh, scene, directory, material_index);
		if (not _materials[material_index]->is_transparent())
			_transparent_mesh_indices.push_back(n_meshes);
		else
			_opaque_mesh_indices.push_back(n_meshes);
		++n_meshes;
	}

	// runs this method by recursion on any children nodes.
	for (uint32_t i = 0; i < node->mNumChildren; ++i)
		_process_node(n_meshes, node->mChildren[i], scene);
}

void ModelResource::get_mesh_indices_by_path(
	std::vector<size_t> &mesh_indices,
	const std::filesystem::path &search_local_path
) const {
	for (size_t i = 0; i < _meshes.size(); ++i) {
		const auto &mat_path = _materials[_meshes[i].material_index()]->path();
		std::string local_path = (
			mat_path.stem().string() + mat_path.extension().string()
			);

		if (search_local_path == local_path)
			mesh_indices.push_back(i);
	}
}

size_t ModelResource::get_material_index_by_path(
	const std::filesystem::path &search_local_path
) const {
	for (size_t i = 0; i < _materials.size(); ++i) {
		const auto &mat_path = _materials[i]->path();
		std::string local_path = (
			mat_path.stem().string() + mat_path.extension().string()
			);
		if (search_local_path == local_path)
			return i;
	}
	return _materials.size();
}

void ModelResource::set_face_cull_option(const GLenum& cull_option) {
	switch (cull_option) {
	case GL_FRONT:
	case GL_BACK:
		_face_cull_option = cull_option;
		break;
	default:
		_face_cull_option = GL_NONE;
	}
}

void ModelResource::draw_transparent_meshes(
	const std::vector<Material::Override> &material_overrides,
	const std::vector<Mesh::Override> &mesh_overrides
) {
	_draw_mesh_by_indices(
		material_overrides, mesh_overrides, _transparent_mesh_indices, false
	);
}

void ModelResource::draw_opaque_meshes(
	const std::vector<Material::Override> &material_overrides,
	const std::vector<Mesh::Override> &mesh_overrides
) {
	_draw_mesh_by_indices(
		material_overrides, mesh_overrides, _opaque_mesh_indices, true
	);
}

void ModelResource::_draw_mesh_by_indices(
	const std::vector<Material::Override> &material_overrides,
	const std::vector<Mesh::Override> &mesh_overrides,
	const std::vector<size_t> &mesh_indices,
	const bool use_face_culling
) {
	if (use_face_culling and _face_cull_option != GL_NONE) {
		glEnable(GL_CULL_FACE);
		glCullFace(_face_cull_option);
	} else {
		glDisable(GL_CULL_FACE);
	}

	// backs up the default Material shared pointers.
	std::vector<Material::Override> backup_materials;
	backup_materials.reserve(material_overrides.size());
	for (const auto &mat_override : material_overrides) {
		backup_materials.emplace_back(
			Material::Override(
				mat_override.material_index,
				_materials[mat_override.material_index]
			)
		);
		_materials[mat_override.material_index] = mat_override.material;
	}

	// draws Meshes.
	size_t mesh_overrides_index = 0;
	std::shared_ptr<Material> last_bound = nullptr;
	for (const size_t &i : mesh_indices) {
		// determines the Material for the Mesh.
		std::shared_ptr<Material> mesh_material = nullptr;
		if (
			    mesh_overrides_index < mesh_overrides.size() 
			and mesh_overrides[mesh_overrides_index].mesh_index == i
		) {
			// applies Mesh Material overriding if given.
			mesh_material = mesh_overrides[mesh_overrides_index].material;
			++mesh_overrides_index;
		} else {
			mesh_material = _materials[_meshes[i].material_index()];
		}

		// binds the Material if it hasn't already, then draws the Mesh.
		if (last_bound != mesh_material) {
			mesh_material->bind_to_GL();
			last_bound = mesh_material;
		}
		_meshes[i].draw();
	}

	// restores the default Material shared pointers.
	for (const auto &mat_backup : backup_materials)
		_materials[mat_backup.material_index] = mat_backup.material;
}
} // namespace gu