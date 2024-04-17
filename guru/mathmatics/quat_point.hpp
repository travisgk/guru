/**
 * quat_point.hpp
 * ---
 * this file defines the QuatPoint class, which defines
 * a point in 3D space that has an Orientation as well.
 *
 * ---
 * any function taking a <factor> will have that
 * <factor> multiplied by the system's delta time.
 *
 */

#pragma once
#include "orientation.hpp"
#include "point.hpp"

namespace gu {
class QuatPoint : public Orientation, public Point {
protected:
	inline QuatPoint(bool CALC_REL_DIRS_WILL_UPDATE)
		: Orientation(CALC_REL_DIRS_WILL_UPDATE) {}

public:
	inline QuatPoint() : Orientation(true) {}
	inline void move_right(const float &factor) { move(_right, factor); }
	inline void move_up(const float &factor) { move(_up, factor); }
	inline void move_forward(const float &factor) {
		move(_forward, factor);
	}

	// sets the orientation to look at the given point
	// with the rotation following an absolute Y-axis.
	inline void look_at(const glm::dvec3 &position) {
		//_quat = glm::quat_cast(glm::lookAt(_position, position, Y_AXIS));
		_quat = glm::quatLookAt(glm::normalize(_position - position), Y_AXIS);
		_set_orientation_as_modified();
	}

	// sets the orientation to look at the given point, with the rotation 
	// using the object's relative up direction as its Y-axis.
	inline void billboard_at(const glm::dvec3 &position) {
		_quat = glm::quatLookAt(glm::normalize(position - _position), _up);
		_set_orientation_as_modified();
	}
};
} // namespace gu