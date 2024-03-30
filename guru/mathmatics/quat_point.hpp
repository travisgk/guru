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
#include "../system/time.hpp"

namespace gu {
class QuatPoint : public Orientation {
protected:
	glm::dvec3 _position = glm::dvec3(0.0, 0.0, 0.0);
	bool _position_is_new = true;

public:
	inline QuatPoint(bool CALC_REL_DIRS_WILL_UPDATE = true)
		: Orientation(CALC_REL_DIRS_WILL_UPDATE) {}
	inline const double &x() const { return _position.x; }
	inline const double &y() const { return _position.y; }
	inline const double &z() const { return _position.z; }
	inline const glm::dvec3 &position() const { return _position; }
	inline void move_x(const float &factor) {
		set_x(_position.x + factor * Delta::get());
	}
	inline void move_y(const float &factor) {
		set_y(_position.y + factor * Delta::get());
	}
	inline void move_z(const float &factor) {
		set_z(_position.z + factor * Delta::get());
	}
	inline void move_right(const float &factor) { move(_right, factor); }
	inline void move_up(const float &factor) { move(_up, factor); }
	inline void move_forward(const float &factor) {
		move(_forward, factor);
	}
	inline void place(const double &x, const double &y, const double &z) {
		place(glm::dvec3(x, y, z));
	}
	void move(const glm::dvec3 &direction, const float &factor);
	void place(const glm::dvec3 &position);
	void set_x(const double &x);
	void set_y(const double &y);
	void set_z(const double &z);

private:
	// this is called by any method that modifies the <_position>.
	void _set_position_as_modified();
};
}