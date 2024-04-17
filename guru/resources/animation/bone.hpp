/**
 * bone.hpp
 * ---
 * this file defines the Bone class that's within the Animation class.
 *
 */

#pragma once
#include "animation.hpp"

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
class Animation::Bone {
private:
	int _bone_ID;
	std::string _name;
	std::vector<PositionKeyframe> _position_keyframes;
	std::vector<OrientationKeyframe> _orientation_keyframes;
	std::vector<ScalingKeyframe> _scaling_keyframes;
	size_t _n_position_keyframes = 0;
	size_t _n_orientation_keyframes = 0;
	size_t _n_scaling_keyframes = 0;
	glm::mat4 _transform_mat;

public:
	Bone(const std::string & name, int bone_ID, const aiNodeAnim * channel);
	inline const int &get_bone_ID() const { return _bone_ID; }
	inline const std::string &get_name() const { return _name; }
	inline const glm::mat4 &get_local_transform_mat() const {
		return _transform_mat;
	}
	void update_matrix(const double &animation_time);

private:
	template <typename T>
	size_t find_keyframe_index(
		const std::vector<T> &keyframes,
		const size_t & n_keyframes,
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
} // namespace gu