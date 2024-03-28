#include "quat_point.hpp"
#include "../system/settings.hpp"

namespace gu {
void QuatPoint::move(const glm::vec3& direction, const float& factor) {
	_position.x += static_cast<float>(direction.x * factor * Delta::get());
	_position.y += static_cast<float>(direction.y * factor * Delta::get());
	_position.z += static_cast<float>(direction.z * factor * Delta::get());
	_set_position_as_modified();
}

void QuatPoint::place(const glm::vec3& position) {
	_position = position;
	_set_position_as_modified();
}

void QuatPoint::set_x(const float& x) {
	_position.x = x;
	_set_position_as_modified();
}

void QuatPoint::set_y(const float& y) {
	_position.y = y;
	_set_position_as_modified();
}

void QuatPoint::set_z(const float& z) {
	_position.z = z;
	_set_position_as_modified();
}

void QuatPoint::_set_position_as_modified() {
	_position_is_new = true;
	#if defined(GURU_AUTO_UPDATE_MATH_OBJECTS)
	update();
	#endif
}
}