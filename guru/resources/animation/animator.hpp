/**
 * animator.hpp
 * ---
 * this file defines the Animator class.
 *
 */

#pragma once
#include "animation.hpp"
#include "bone.hpp"

namespace gu {
class Animator {
private:
	std::vector<glm::mat4> _final_bone_matrices;
	Animation *_animation;
	double _current_time;

public:
	Animator();
	Animator(Animation &animation);

private:
	void _setup_bone_matrices();

public:
	inline const std::vector<glm::mat4> &get_final_bone_matrices() const {
		return _final_bone_matrices;
	}
	void set_animation(Animation &animation);
	void update_animation();

private:
	// calculates the final bone matrices
	// for every bone in the rigging hierarchy by recursion.
	void _calc_bone_transforms(
		const Animation::AssimpNodeData *node,
		const glm::mat4 &parent_transform
	);


public:
	inline void print_rig_hierarchy() {
		_print_assimp_node_info(_animation->get_root_node());
	}

private:
	void _print_assimp_node_info(
		const Animation::AssimpNodeData &node, uint8_t current_depth = 0
	);
};
} // namespace gu