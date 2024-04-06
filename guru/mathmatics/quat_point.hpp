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
public:
	inline QuatPoint(bool CALC_REL_DIRS_WILL_UPDATE = true)
		: Orientation(CALC_REL_DIRS_WILL_UPDATE) {}
	inline void move_right(const float &factor) { move(_right, factor); }
	inline void move_up(const float &factor) { move(_up, factor); }
	inline void move_forward(const float &factor) {
		move(_forward, factor);
	}
};
} // namespace gu