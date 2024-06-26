#include "point.hpp"

namespace gu {
void Point::move(const glm::dvec3 &direction, const float &factor) {
	_position.x += direction.x * factor * Delta::get();
	_position.y += direction.y * factor * Delta::get();
	_position.z += direction.z * factor * Delta::get();
	_set_position_as_modified();
}

void Point::place(const glm::dvec3 &position) {
	_position = position;
	_set_position_as_modified();
}

void Point::set_x(const double &x) {
	_position.x = x;
	_set_position_as_modified();
}

void Point::set_y(const double &y) {
	_position.y = y;
	_set_position_as_modified();
}

void Point::set_z(const double &z) {
	_position.z = z;
	_set_position_as_modified();
}
}