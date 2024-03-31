#include "model_resource.hpp"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "material_list.hpp"

namespace gu {
ModelResource::ModelResource(
	const std::filesystem::path& path, const GLenum &face_cull_option
) {
	set_face_cull_option(face_cull_option);
	load(path);
}

ModelResource::~ModelResource() {
	for (uint32_t i = 0; i < _meshes.size(); ++i)
		material::MaterialList::delete_material(_meshes[i].material_path());
}

void ModelResource::load(const std::filesystem::path &path) {
	if (not std::filesystem::exists(path)) {
		std::cerr << path << " could not be found." << std::endl;
		return;
	}

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
	_process_node(scene->mRootNode, scene);
}

void ModelResource::_process_node(aiNode* node, const aiScene* scene) {
	std::filesystem::path directory = _path.parent_path();
	for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
		_meshes.push_back(Mesh());
		_meshes[_meshes.size() - 1].load(
			scene->mMeshes[node->mMeshes[i]], scene, directory
		);
	}
	
	for (uint32_t i = 0; i < node->mNumChildren; ++i)
		_process_node(node->mChildren[i], scene);
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
} // namespace gu