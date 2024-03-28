/**
 * orientation.hpp
 * ---
 * this file defines the Orientation class for managing 
 * an object's orientation using a quaternion.
 * 
 * ---
 * any function taking a <factor> will have that
 * <factor> multiplied by the system's delta time.
 * 
 */

#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace gu {
class Orientation {
protected:
	static const glm::vec3 X_AXIS, Y_AXIS, Z_AXIS; // absolute axes
	const bool CALC_REL_DIRS_WILL_UPDATE; // update setting
	glm::quat _quat = glm::quat(1.0, 0.0, 0.0, 0.0); // rotation
	bool _orientation_is_new = true; // update condition
	glm::vec3 _right = glm::vec3(1.0, 0.0, 0.0); // relative right
	glm::vec3 _up = glm::vec3(0.0, 1.0, 0.0); // relative up
	glm::vec3 _forward = glm::vec3(0.0, 0.0, 1.0); // relative forward

public:
	// ctor. the parameter determines if updating the relative directions
	// will make this Orientation considered as updated.
	inline Orientation(bool CALC_REL_DIRS_UPDATES = true)
		: CALC_REL_DIRS_WILL_UPDATE(CALC_REL_DIRS_UPDATES)
	{}

	// returns the quaternion representing the rotation.
	inline const glm::quat& get() const { return _quat; }
	inline const glm::vec3& right() const { return _right; }
	inline const glm::vec3& up() const { return _up; }
	inline const glm::vec3& forward() const { return _forward; }

	// returns the pitch of the rotation quaternion in radians.
	inline float calc_pitch() const {
		return atan2(
			2.0f * (_quat.x * _quat.w + _quat.y * _quat.z),
			1.0f - 2.0f * (_quat.x * _quat.x + _quat.z * _quat.z)
		);
	}

	// returns the yaw of the rotation quaternion in radians.
	inline float calc_yaw() const {
		return asin(2.0f * (_quat.x * _quat.y + _quat.z * _quat.w));
	}

	// returns the roll of the rotation quaternion in radians.
	inline float calc_roll() const {
		return atan2(
			2.0f * (_quat.z * _quat.w + _quat.x * _quat.y),
			1.0f - 2.0f * (_quat.y * _quat.y + _quat.z * _quat.z)
		);
	}

	// sets the rotation with the given quaternion.
	inline void orient(const glm::quat& quat) {
		_quat = quat;
		_set_orientation_as_modified();
	}
	inline void rotate_x(const float& factor) { rotate(X_AXIS, factor); }
	inline void rotate_y(const float& factor) { rotate(Y_AXIS, factor); }
	inline void rotate_z(const float& factor) { rotate(Z_AXIS, factor); }
	inline void add_pitch(const float& factor) { rotate(_right, factor); }
	inline void add_yaw(const float& factor) { rotate(_up, factor); }
	inline void add_roll(const float& factor) { rotate(_forward, factor); }
	
	// rotates the orientation around the given <axis> 
	// by the given <factor>.
	void rotate(const glm::vec3& axis, const float& factor);

	// updates the object's matrices. 
	// if the compiler flag GURU_AUTO_UPDATE_MATH_OBJECTS is not used,
	// then this should be called on every frame, 
	// or whenever the object's attributes were modified.
	inline virtual void update() { _update_relative_directions(); }

private:
	// this is called by any method that modifies the rotation quaternion.
	void _set_orientation_as_modified();

protected:
	void _update_relative_directions();
};
}