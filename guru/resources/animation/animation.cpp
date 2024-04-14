#include "animation.hpp"
#include <iostream>
#include <assimp/anim.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../model/assimp_to_glm.hpp"

namespace gu {
Animation::Animation(const std::filesystem::path &animation_path, ModelResource &model_res) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(
		animation_path.string(), aiProcess_Triangulate
	);
	if (not scene) {
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
	_n_ticks_per_second = ai_animation->mTicksPerSecond;
	//aiMatrix4x4 global_transformation = scene->mRootNode->mTransformation; // don't know why this is in reference source
	//global_transformation = global_transformation.Inverse();
	_read_hierarchy_data(_root_node, scene->mRootNode);
	_read_missing_bones(ai_animation, model_res);
}

Animation::Bone *Animation::find_bone(const std::string &name) {
	auto iter = std::find_if(
		_bones.begin(), 
		_bones.end(),
		[&](const Bone &bone) {
			return bone.name() == name;
		}
	);
	if (iter == _bones.end())
		return nullptr;
	return &(*iter);
}

void Animation::_read_hierarchy_data(AssimpNodeData &node, const aiNode *src) {
	assert(src);
	node.name = src->mName.data;
	set_mat4(node.transformation, src->mTransformation);
	node.n_children = src->mNumChildren;

	for (size_t i = 0; i < src->mNumChildren; ++i) {
		AssimpNodeData child_node;
		_read_hierarchy_data(child_node, src->mChildren[i]);
		node.children.push_back(child_node);
	}
}

void Animation::_read_missing_bones(
	const aiAnimation *ai_animation, ModelResource &model
) {
	const size_t &n_channels = ai_animation->mNumChannels;
	auto &bone_ID_map = model.bone_ID_map();
	int n_bones = static_cast<int>(bone_ID_map.size());

	for (size_t i = 0; i < n_channels; ++i) {
		const auto &channel = ai_animation->mChannels[i];
		std::string bone_name = channel->mNodeName.data;

		if (bone_ID_map.find(bone_name) == bone_ID_map.end()) {
			bone_ID_map[bone_name].id = n_bones;
			++n_bones;
		}
		_bones.push_back(
			Bone(bone_name, bone_ID_map[bone_name].id, channel)
		);
	}
	_bone_ID_map = bone_ID_map;
}
} // namespace gu