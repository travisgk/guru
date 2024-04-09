#include "transformation.hpp"

namespace gu {
void Transformation::set_scaling(const float &absolute_scale) {
	_scaling = glm::vec3(absolute_scale);
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
	if (not _position_is_new and not _orientation_is_new and not _scaling_is_new)
		return;
	_model_mat = glm::mat4(1.0);
	_model_mat = glm::translate(_model_mat, static_cast<glm::vec3>(_position));
	_model_mat = _model_mat * static_cast<glm::mat4>(glm::toMat4(_quat));
	_model_mat = glm::scale(_model_mat, _scaling);
	_orientation_is_new = false;
	_position_is_new = false;
	_scaling_is_new = false;
}
}