#include "animation.hpp"
#include <iostream>
#include <assimp/anim.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../model/assimp_to_glm.hpp"

namespace gu {
Animation::Animation(
	const std::filesystem::path &animation_path, ModelResource &model_res
) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(
		animation_path.string(), aiProcess_Triangulate
	);
	if (not scene or not scene->mRootNode) {
		std::cerr << animation_path << " could not be loaded." << std::endl;
		return;
	}
	if (scene->mNumAnimations == 0) {
		std::cerr 
			<< animation_path 
			<< " does not contain any animations." 
			<< std::endl;
		return;
	}

	auto &ai_animation = scene->mAnimations[0];
	_duration = ai_animation->mDuration;
	_ticks_per_second = ai_animation->mTicksPerSecond;
	aiMatrix4x4 global_transformation = scene->mRootNode->mTransformation;
	global_transformation = global_transformation.Inverse();
	set_mat4(_global_inverse_transform, global_transformation);

	_read_hierarchy_data(_root_node, scene->mRootNode);
	_create_bones(ai_animation, model_res);
}

Animation::Bone *Animation::find_bone(const std::string &name) {
	for (auto &bone : _bones)
		if (bone.get_name() == name)
			return &bone;

	return nullptr;
}

void Animation::_read_hierarchy_data(AssimpNodeData &node, const aiNode *src) {
	assert(src);
	node.name = src->mName.data;
	glm::mat4 rel_transform_mat;
	set_mat4(rel_transform_mat, src->mTransformation);
	node.rel_transform_mat = rel_transform_mat;
	node.n_children = src->mNumChildren;

	for (size_t i = 0; i < src->mNumChildren; ++i) {
		AssimpNodeData child_node;
		_read_hierarchy_data(child_node, src->mChildren[i]);
		node.children.push_back(child_node);
	}
}

void Animation::_create_bones(
	const aiAnimation *ai_animation, ModelResource &model
) {
	const size_t &n_channels = ai_animation->mNumChannels;
	auto &name_to_rig_info = model.name_to_rig_info();
	int n_bones = static_cast<int>(model.name_to_rig_info().size());

	for (size_t i = 0; i < n_channels; ++i) {
		const auto &channel = ai_animation->mChannels[i];
		std::string bone_name = channel->mNodeName.data;

		if (name_to_rig_info.find(bone_name) == name_to_rig_info.end()) {
			name_to_rig_info[bone_name].bone_ID = n_bones;
			++n_bones;
		}
		_bones.push_back(
			Bone(bone_name, name_to_rig_info[bone_name].bone_ID, channel)
		);
	}

	// this Animation makes its own copy 
	// of the rigging map from the ModelResource.
	_name_to_rig_info = name_to_rig_info;
}
} // namespace gu