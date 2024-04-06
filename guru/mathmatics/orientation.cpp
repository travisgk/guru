#include "orientation.hpp"
#include "../system/time.hpp"

namespace gu {
const glm::dvec3 Orientation::X_AXIS = glm::dvec3(1.0, 0.0, 0.0);
const glm::dvec3 Orientation::Y_AXIS = glm::dvec3(0.0, 1.0, 0.0);
const glm::dvec3 Orientation::Z_AXIS = glm::dvec3(0.0, 0.0, 1.0);

void Orientation::rotate(const glm::dvec3 &axis, const float &factor) {
	_quat = glm::angleAxis(
		factor * Delta::get(), axis
	) * _quat;
	_set_orientation_as_modified();
}

void Orientation::_set_orientation_as_modified() {
	_orientation_is_new = true;
	#if defined(GURU_AUTO_UPDATE_MATH_OBJECTS)
	update();
	#endif
}

void Orientation::update() { 
	_update_relative_directions(); 
}

void Orientation::_update_relative_directions() {
	if (not _orientation_is_new)
		return;
	_forward.x = 2.0 * (_quat.x * _quat.z + _quat.w * _quat.y);
	_forward.y = 2.0 * (_quat.y * _quat.z - _quat.w * _quat.x);
	_forward.z = 1.0 - (_quat.x * _quat.x + _quat.y * _quat.y) * 2.0;
	_forward = glm::normalize(_forward);

	_up.x = 2.0 * (_quat.x * _quat.y - _quat.w * _quat.z);
	_up.y = 1.0 - (_quat.x * _quat.x + _quat.z * _quat.z) * 2.0;
	_up.z = 2.0 * (_quat.y * _quat.z + _quat.w * _quat.x);
	_up = glm::normalize(_up);

	_right.x = -1.0 * (
		1.0 - (_quat.y * _quat.y + _quat.z * _quat.z) * 2.0
	);
	_right.y = -2.0 * (_quat.x * _quat.y + _quat.w * _quat.z);
	_right.z = -2.0 * (_quat.x * _quat.z - _quat.w * _quat.y);
	_right = glm::normalize(_right);

	if (CALC_REL_DIRS_WILL_UPDATE)
		_orientation_is_new = false;
}
} // namespace gu