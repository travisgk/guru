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
	glm::vec3 _position = glm::vec3(0.0, 0.0, 0.0);
	bool _position_is_new = true;

public:
	inline QuatPoint(bool CALC_REL_DIRS_WILL_UPDATE = true)
		: Orientation(CALC_REL_DIRS_WILL_UPDATE) {}
	inline const float& x() const { return _position.x; }
	inline const float& y() const { return _position.y; }
	inline const float& z() const { return _position.z; }
	inline const glm::vec3& position() const { return _position; }
	inline void move_x(const float& factor) {
		set_x(static_cast<float>(_position.x + factor * Delta::get()));
	}
	inline void move_y(const float& factor) {
		set_y(static_cast<float>(_position.y + factor * Delta::get()));
	}
	inline void move_z(const float& factor) {
		set_z(static_cast<float>(_position.z + factor * Delta::get()));
	}
	inline void move_right(const float& factor) { move(_right, factor); }
	inline void move_up(const float& factor) { move(_up, factor); }
	inline void move_forward(const float& factor) {
		move(_forward, factor);
	}
	inline void place(const float& x, const float& y, const float& z) {
		place(glm::vec3(x, y, z));
	}
	void move(const glm::vec3& direction, const float& factor);
	void place(const glm::vec3& position);
	void set_x(const float& x);
	void set_y(const float& y);
	void set_z(const float& z);

private:
	// this is called by any method that modifies the <_position>.
	void _set_position_as_modified();
};
}