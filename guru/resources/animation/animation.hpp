/**
 * animation.hpp
 * ---
 * this file defines the Animation class.
 *
 */

#pragma once
#include <vector>
#include <string>
#include "../model/model_resource.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace gu {
class Animation {
public:
	class Bone; // forward declaration of Bone class.

	struct AssimpNodeData {
		glm::mat4 rel_transform_mat = glm::mat4(1.0); // relative to parent
		std::string name;
		size_t n_children = 0;
		std::vector<AssimpNodeData> children;
	};

	double _duration = 0.0;
	double _ticks_per_second = 0.0;
	AssimpNodeData _root_node;
	glm::mat4 _global_inverse_transform = glm::mat4(1.0f);
	std::vector<Bone> _bones;
	std::map<std::string, Mesh::RigInfo> _name_to_rig_info; // ModelResource copy

	Animation(const std::filesystem::path &animation_path, ModelResource &model_res);

	Bone *find_bone(const std::string& name);

	inline const double &get_duration() const { return _duration; }
	inline const double &get_ticks_per_second() const { return _ticks_per_second; }
	inline const AssimpNodeData &get_root_node() const { return _root_node; }
	inline std::map<std::string, Mesh::RigInfo> &get_name_to_rig_info() {
		return _name_to_rig_info;
	}

	inline const glm::mat4 &get_global_inverse_transform() const {
		return _global_inverse_transform;
	}

private:
	void _read_hierarchy_data(AssimpNodeData &node, const aiNode *src);

	void _create_bones(const aiAnimation *ai_animation, ModelResource &model);
};
} // namespace gu

#include "bone.hpp" // import at bottom to avoid circular import