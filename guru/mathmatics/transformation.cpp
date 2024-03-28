#include "transformation.hpp"
#include "../system/settings.hpp"

namespace gu {
void Transformation::set_scaling(const float& absolute_scale) {
	_scaling = glm::vec3(absolute_scale, absolute_scale, absolute_scale);
	_scaling_is_new = true;
	#if defined(GURU_AUTO_UPDATE_MATH_OBJECTS)
	update();
	#endif
}

void Transformation::update() {
	_update_relative_directions();
	_update_model_matrix();
}

void Transformation::_update_model_matrix() {
	if (not(_position_is_new or _orientation_is_new or _scaling_is_new))
		return;
	_model_matrix = glm::mat4(1.0);
	_model_matrix = glm::translate(_model_matrix, _position);
	_model_matrix = _model_matrix * glm::toMat4(_quat);
	_model_matrix = glm::scale(_model_matrix, _scaling);
	_orientation_is_new = false;
	_position_is_new = false;
	_scaling_is_new = false;
}
}