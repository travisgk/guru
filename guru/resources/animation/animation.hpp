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
		glm::mat4 transform_mat = glm::mat4(1.0); // UNCERTAIN: only used as a fallback? maybe unnecessary
		std::string name;
		size_t n_children = 0;
		std::vector<AssimpNodeData> children;
	};
	
	Animation(const std::filesystem::path &animation_path, ModelResource &model_res);

	Bone *find_bone(const std::string& name);

	inline const double &duration() const { return _duration; }
	inline const double &n_ticks_per_second() const { return _n_ticks_per_second; }
	inline const AssimpNodeData &get_root_node() const { return _root_node; }
	inline std::map<std::string, Mesh::BoneInfo> &get_bone_ID_map() {
		return _bone_ID_map;
	}

private:
	void _read_hierarchy_data(AssimpNodeData &node, const aiNode *src);

	void _read_missing_bones(const aiAnimation *ai_animation, ModelResource &model);

	double _duration = 0.0;
	double _n_ticks_per_second = 0.0;
	std::vector<Bone> _bones;
	AssimpNodeData _root_node;
	std::map<std::string, Mesh::BoneInfo> _bone_ID_map; // ModelResource copy
};
} // namespace gu

#include "bone.hpp" // import at bottom to avoid circular import