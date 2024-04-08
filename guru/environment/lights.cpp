#include "lights.hpp"

namespace gu {
void DirLight::_set_orientation_as_modified() {
	_orientation_is_new = true;
	_orientation_needs_GL_update = true;
	#if defined(GURU_AUTO_UPDATE_MATH_OBJECTS)
	update();
	#endif
}

void PointLight::_set_position_as_modified() {
	_position_is_new = true;
	_position_needs_GL_update = true;
}

void SpotLight::_set_orientation_as_modified() {
	_orientation_is_new = true;
	_orientation_needs_GL_update = true;
	#if defined(GURU_AUTO_UPDATE_MATH_OBJECTS)
	update();
	#endif
}

void SpotLight::_set_position_as_modified() {
	_position_is_new = true;
	_position_needs_GL_update = true;
	#if defined(GURU_AUTO_UPDATE_MATH_OBJECTS)
	update();
	#endif
}
} // namespace gu