#include "animator.hpp"
#include "../../system/time.hpp"

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
	_current_time += _animation->n_ticks_per_second() * gu::Delta::get();
	_current_time = fmod(_current_time, _animation->duration());
	_calc_bone_transforms(&_animation->get_root_node(), glm::mat4(1.0f));
}

// calculates the final bone matrices 
// for every bone in the rigging hierarchy by recursion.
void Animator::_calc_bone_transforms(
	const Animation::AssimpNodeData *node,
	const glm::mat4 &parent_transform
) {
	Animation::Bone *bone = _animation->find_bone(node->name);

	glm::mat4 global_tf = parent_transform;

	if (bone) {
		bone->update_matrix(_current_time);
		global_tf = global_tf * bone->get_local_transform_mat();
	} else {
		global_tf = global_tf * node->transform_mat;
	}
	auto &bone_ID_map = _animation->get_bone_ID_map();
	if (bone_ID_map.find(node->name) != bone_ID_map.end()) {
		const Mesh::BoneInfo &bone_info = bone_ID_map[node->name];
		const glm::mat4 &offset = bone_info.offset;
		_final_bone_matrices[bone_info.id] = global_tf * offset;
	}

	for (size_t i = 0; i < node->n_children; ++i)
		_calc_bone_transforms(&node->children[i], global_tf);
}
} // namespace gu