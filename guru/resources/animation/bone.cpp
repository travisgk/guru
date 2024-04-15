#include "bone.hpp"
#include <iostream>
#include <assimp/anim.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../model/assimp_to_glm.hpp"

namespace gu {
Animation::Bone::Bone(
	const std::string &name, int bone_ID, const aiNodeAnim *channel
) : _name(name),
    _bone_ID(bone_ID),
    _transform_mat(1.0f)
{
	_n_position_keyframes = channel->mNumPositionKeys;
	for (size_t i = 0; i < _n_position_keyframes; ++i) {
		PositionKeyframe position_keyframe;
		const aiVector3D &ai_position = channel->mPositionKeys[i].mValue;
		set_vec3(position_keyframe.position, ai_position);
		position_keyframe.time_stamp = channel->mPositionKeys[i].mTime;
		_position_keyframes.push_back(position_keyframe);
	}

	_n_orientation_keyframes = channel->mNumRotationKeys;
	for (size_t i = 0; i < _n_orientation_keyframes; ++i) {
		OrientationKeyframe orientation_keyframe;
		const aiQuaternion &ai_quat = channel->mRotationKeys[i].mValue;
		set_quat(orientation_keyframe.orientation, ai_quat);
		orientation_keyframe.time_stamp = channel->mRotationKeys[i].mTime;
		_orientation_keyframes.push_back(orientation_keyframe);
	}

	_n_scaling_keyframes = channel->mNumScalingKeys;
	for (size_t i = 0; i < _n_scaling_keyframes; ++i) {
		ScalingKeyframe scaling_keyframe;
		const aiVector3D &ai_scaling = channel->mScalingKeys[i].mValue;
		set_vec3(scaling_keyframe.scaling, ai_scaling);
		scaling_keyframe.time_stamp = channel->mScalingKeys[i].mTime;
		_scaling_keyframes.push_back(scaling_keyframe);
	}
}

void Animation::Bone::update_matrix(const double &animation_time) {
	glm::vec3 position = _interpolated_position(animation_time);
	glm::quat orientation = _interpolated_orientation(animation_time);
	glm::vec3 scaling = _interpolated_scaling(animation_time);

	_transform_mat = glm::mat4(1.0);
	for (uint8_t i = 0; i < 3; ++i)
		_transform_mat[3][i] = position[i];

	glm::mat4 rot_mat = glm::toMat4(orientation);
	for (uint8_t i = 0; i < 3; ++i)
		for (uint8_t j = 0; j < 3; ++j)
			_transform_mat[i][j] = rot_mat[i][j];

	for (uint8_t i = 0; i < 3; ++i)
		for (uint8_t j = 0; j < 3; ++j)
			_transform_mat[i][j] = _transform_mat[i][j] * scaling[j];
}

static double calc_progress_factor(
	const double &prev_time_stamp, 
	const double &next_time_stamp, 
	const double &animation_time
) {
	double progression = animation_time - prev_time_stamp;
	double duration = next_time_stamp - prev_time_stamp;
	return progression / duration;
}

glm::vec3 Animation::Bone::_interpolated_position(const double &animation_time) {
	if (_n_position_keyframes == 1)
		return _position_keyframes[0].position;

	size_t p0_index = get_keyframe_index(
		_position_keyframes, _n_position_keyframes, animation_time
	);
	size_t p1_index = p0_index + 1;
	double progress_factor = calc_progress_factor(
		_position_keyframes[p0_index].time_stamp,
		_position_keyframes[p1_index].time_stamp,
		animation_time
	);

	glm::vec3 interpolated_position = glm::mix(
		_position_keyframes[p0_index].position,
		_position_keyframes[p1_index].position,
		progress_factor
	);
	return interpolated_position;
}

glm::quat Animation::Bone::_interpolated_orientation(const double &animation_time) {
	if (_n_orientation_keyframes == 1)
		return glm::normalize(_orientation_keyframes[0].orientation);

	size_t k0_index = get_keyframe_index(
		_orientation_keyframes, _n_orientation_keyframes, animation_time
	);
	size_t k1_index = k0_index + 1;
	double progress_factor = calc_progress_factor(
		_orientation_keyframes[k0_index].time_stamp,
		_orientation_keyframes[k1_index].time_stamp,
		animation_time
	);

	glm::quat interpolated_orientation = glm::slerp(
		_orientation_keyframes[k0_index].orientation,
		_orientation_keyframes[k1_index].orientation,
		static_cast<float>(progress_factor)
	);
	return glm::normalize(interpolated_orientation);
}

glm::vec3 Animation::Bone::_interpolated_scaling(const double &animation_time) {
	if (_n_scaling_keyframes == 1)
		return _scaling_keyframes[0].scaling;

	size_t k0_index = get_keyframe_index(
		_scaling_keyframes, _n_scaling_keyframes, animation_time
	);
	size_t k1_index = k0_index + 1;
	double progress_factor = calc_progress_factor(
		_scaling_keyframes[k0_index].time_stamp,
		_scaling_keyframes[k1_index].time_stamp,
		animation_time
	);

	glm::vec3 interpolated_scaling = glm::mix(
		_scaling_keyframes[k0_index].scaling,
		_scaling_keyframes[k1_index].scaling,
		progress_factor
	);
	return interpolated_scaling;
}
} // namespace gu