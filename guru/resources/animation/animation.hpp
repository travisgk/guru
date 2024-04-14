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

namespace {
struct PositionKeyframe {
	glm::vec3 position = glm::vec3(0.0f);
	double time_stamp = 0.0;	  
};

struct OrientationKeyframe {
	glm::quat orientation = glm::quat();
	double time_stamp = 0.0;
};

struct ScalingKeyframe {
	glm::vec3 scaling = glm::vec3(1.0f);
	double time_stamp = 0.0;
};
} // blank namespace

namespace gu {
class Animation {
public:
	class Bone {
	private:
		int _bone_ID;
		std::string _name;
		std::vector<PositionKeyframe> _position_keyframes;
		std::vector<OrientationKeyframe> _orientation_keyframes;
		std::vector<ScalingKeyframe> _scaling_keyframes;
		size_t _n_position_keyframes = 0;
		size_t _n_orientation_keyframes = 0;
		size_t _n_scaling_keyframes = 0;
		glm::mat4 _bone_transform;

	public:
		Bone(const std::string &name, int bone_ID, const aiNodeAnim *channel);
		inline const int &bone_ID() const { return _bone_ID; }
		inline const std::string &name() const { return _name; }
		inline const glm::mat4 &get_model_matrix() const { return _bone_transform; }
		void update_matrix(const double &animation_time);

	private:
		template <typename T>
		size_t get_keyframe_index(
			const std::vector<T> &keyframes,
			const size_t &n_keyframes,
			const double &animation_time
		) {
			for (size_t i = 0; i < n_keyframes - 1; ++i) {
				if (animation_time < keyframes[i + 1].time_stamp)
					return i;
			}
			assert(0);
			return 0;
		}
		glm::vec3 _interpolated_position(const double &animation_time);
		glm::quat _interpolated_orientation(const double &animation_time);
		glm::vec3 _interpolated_scaling(const double &animation_time);
	};

	struct AssimpNodeData {
		glm::mat4 transformation = glm::mat4(1.0); // UNCERTAIN: only used as a fallback? maybe unnecessary
		std::string name;
		size_t n_children = 0;
		std::vector<AssimpNodeData> children;
	};
	
	Animation(const std::filesystem::path &animation_path, ModelResource &model_res);

	Bone *find_bone(const std::string& name);

	inline const double &duration() const { return _duration; }
	inline const double &n_ticks_per_second() const { return _n_ticks_per_second; }
	inline const AssimpNodeData &get_root_node() const { return _root_node; }
	inline const std::map<std::string, Mesh::BoneInfo> &get_bone_ID_map() {
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