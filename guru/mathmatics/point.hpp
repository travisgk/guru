/**
 * point.hpp
 * ---
 * this file defines the Point class, which defines
 * a point in 3D space.
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
class Point {
protected:
	glm::dvec3 _position = glm::dvec3(0.0, 0.0, 0.0);
	bool _position_is_new = true;

public:
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

	inline void place(const double &x, const double &y, const double &z) {
		place(glm::dvec3(x, y, z));
	}
	void move(const glm::dvec3 &direction, const float &factor);
	void place(const glm::dvec3 &position);
	void set_x(const double &x);
	void set_y(const double &y);
	void set_z(const double &z);

protected:
	// this is called by any method that modifies the position.
	virtual void _set_position_as_modified();
};
}