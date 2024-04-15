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
public:
	Animator();
	Animator(Animation &animation);

private:
	void _setup_bone_matrices();

public:
	inline const std::vector<glm::mat4> &final_bone_matrices() const {
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

	std::vector<glm::mat4> _final_bone_matrices;
	Animation *_animation;
	double _current_time;
};
} // namespace gu