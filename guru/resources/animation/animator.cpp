#include "animator.hpp"
#include "../../system/time.hpp"
#include <iostream>

namespace gu {
Animator::Animator() {
	_setup_bone_matrices();
}

Animator::Animator(Animation &animation) {
	_setup_bone_matrices();
	set_animation(animation);
}

void Animator::_setup_bone_matrices() {
	_final_bone_matrices.reserve(Settings::MAX_BONES);
	for (size_t i = 0; i < Settings::MAX_BONES; ++i)
		_final_bone_matrices.push_back(glm::mat4(1.0f));
}

void Animator::set_animation(Animation &animation) {
	_current_time = 0.0;
	_animation = &animation;
}

void Animator::update_animation() {
	if (not _animation)
		return;
	_current_time += _animation->get_ticks_per_second() * gu::Delta::get();
	_current_time = fmod(_current_time, _animation->get_duration());
	_calc_bone_transforms(&_animation->get_root_node(), glm::mat4(1.0f));
}

// recursive method.
void Animator::_calc_bone_transforms(
	const Animation::AssimpNodeData *node,
	const glm::mat4 &parent_transform
) {
	glm::mat4 global_tf = parent_transform;
	glm::mat4 node_tf = node->rel_transform_mat;
	Animation::Bone *bone = _animation->find_bone(node->name);

	if (bone) {
		bone->update_matrix(_current_time);
		node_tf = bone->get_local_transform_mat();
	}

	global_tf = global_tf * node_tf;

	auto &name_to_rig_info = _animation->get_name_to_rig_info();
	if (name_to_rig_info.find(node->name) != name_to_rig_info.end()) {
		const Mesh::RigInfo &rig_info = name_to_rig_info[node->name];
		_final_bone_matrices[rig_info.bone_ID] = (
			global_tf * rig_info.local_space_to_bone
		);
	}

	for (size_t i = 0; i < node->n_children; ++i)
		_calc_bone_transforms(&node->children[i], global_tf);
}

void Animator::_print_assimp_node_info(
	const Animation::AssimpNodeData &node, uint8_t current_depth
) {
	for (uint8_t i = 0; i < current_depth; ++i)
		std::cout << "     ";
	std::cout << node.name;
	auto &rig_info = _animation->get_name_to_rig_info();
	if (rig_info.find(node.name) != rig_info.end()) {
		std::cout << "     " << rig_info[node.name].bone_ID << std::endl;
		for (uint8_t i = 0; i < current_depth; ++i)
			std::cout << "     ";
		std::cout << "local_space_to_bone:" << std::endl;
		for (uint8_t r = 0; r < 4; ++r) {
			for (uint8_t i = 0; i < current_depth; ++i)
				std::cout << "     ";
			for (uint8_t c = 0; c < 4; ++c)
				std::cout
					<< std::setw(6) << std::right << std::setprecision(3)
					<< std::fixed
					<< rig_info[node.name].local_space_to_bone[c][r]
					<< '\t';
			std::cout << std::endl;
		}
		std::cout << std::endl;
		for (uint8_t i = 0; i < current_depth; ++i)
			std::cout << "     ";
		std::cout << "final bone matrix:" << std::endl;
		for (uint8_t r = 0; r < 4; ++r) {
			for (uint8_t i = 0; i < current_depth; ++i)
				std::cout << "     ";
			for (uint8_t c = 0; c < 4; ++c)
				std::cout
					<< std::setw(6) << std::right << std::setprecision(3)
					<< std::fixed
					<< _final_bone_matrices[rig_info[node.name].bone_ID][c][r]
					<< '\t';
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;

	for (const auto &child : node.children)
		_print_assimp_node_info(child, current_depth + 1);
}
} // namespace gu