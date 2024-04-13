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
	static const glm::dvec3 X_AXIS, Y_AXIS, Z_AXIS; // absolute axes
	const bool CALC_REL_DIRS_WILL_UPDATE; // update setting
	glm::dquat _quat = glm::dquat(1.0, 0.0, 0.0, 0.0); // rotation
	bool _orientation_is_new = true; // update condition
	glm::dvec3 _right = glm::dvec3(1.0, 0.0, 0.0); // relative right
	glm::dvec3 _up = glm::dvec3(0.0, 1.0, 0.0); // relative up
	glm::dvec3 _forward = glm::dvec3(0.0, 0.0, 1.0); // relative forward

	// ctor. the parameter determines if updating the relative directions
	// will make this Orientation considered as updated.
	inline Orientation(bool CALC_REL_DIRS_UPDATES)
		: CALC_REL_DIRS_WILL_UPDATE(CALC_REL_DIRS_UPDATES)
	{}

public:
	inline Orientation() : CALC_REL_DIRS_WILL_UPDATE(true) {}

	// returns the quaternion representing the rotation.
	inline const glm::dquat &get_quat() const { return _quat; }
	inline const glm::dvec3 &right() const { return _right; }
	inline const glm::dvec3 &up() const { return _up; }
	inline const glm::dvec3 &forward() const { return _forward; }

	// returns the pitch of the rotation quaternion in radians.
	inline double calc_pitch() const {
		return atan2(
			2.0 * (_quat.x * _quat.w + _quat.y * _quat.z),
			1.0 - 2.0 * (_quat.x * _quat.x + _quat.z * _quat.z)
		);
	}

	// returns the yaw of the rotation quaternion in radians.
	inline double calc_yaw() const {
		return asin(2.0 * (_quat.x * _quat.y + _quat.z * _quat.w));
	}

	// returns the roll of the rotation quaternion in radians.
	inline double calc_roll() const {
		return atan2(
			2.0 * (_quat.z * _quat.w + _quat.x * _quat.y),
			1.0 - 2.0 * (_quat.y * _quat.y + _quat.z * _quat.z)
		);
	}

	// sets the rotation using a given direction vector.
	void orient(const glm::dvec3 &direction);

	// sets the rotation using pitch, yaw, and roll in radians.
	void orient(const double &pitch, const double &yaw, const double &roll);

	// sets the rotation with the given quaternion.
	inline void orient(const glm::dquat &quat) {
		_quat = quat;
		_set_orientation_as_modified();
	}

	inline void set_pitch(const double &radians) {
		orient(radians, calc_yaw(), calc_roll());
	}

	inline void set_yaw(const double &radians) {
		orient(calc_pitch(), radians, calc_roll());
	}

	inline void set_roll(const double &radians) {
		orient(calc_pitch(), calc_yaw(), radians);
	}

	inline void rotate_x(const float &factor) { rotate(X_AXIS, factor); }
	inline void rotate_y(const float &factor) { rotate(Y_AXIS, factor); }
	inline void rotate_z(const float &factor) { rotate(Z_AXIS, factor); }
	inline void add_pitch(const float &factor) { rotate(_right, factor); }
	inline void add_yaw(const float &factor) { rotate(_up, factor); }
	inline void add_roll(const float &factor) { rotate(_forward, factor); }
	
	// rotates the orientation around the given <axis> 
	// by the given <factor>.
	void rotate(const glm::dvec3 &axis, const float &factor);

protected:
	// this is called by any method that modifies the rotation quaternion.
	virtual void _set_orientation_as_modified();

public:
	// updates the object's matrices.
	// ---
	// if the compiler flag GURU_AUTO_UPDATE_MATH_OBJECTS is not used,
	// then this should be called on every frame, 
	// or whenever the object's attributes were modified.
	virtual void update();

protected:
	// updates the 3D vectors for the Orientation's 
	// relative right, up, and forward directions
	// to match the new internal rotation quaternion.
	void _update_relative_directions();
};
}