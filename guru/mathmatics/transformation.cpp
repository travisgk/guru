#include "transformation.hpp"
#include "../system/settings.hpp"

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

static void rotate(
	glm::mat4 &mod_mat, const glm::mat4 mat, const glm::dquat &quat
) {
	glm::mat4 rot = static_cast<glm::mat4>(glm::toMat4(quat));

}

void Transformation::_update_model_matrix() {
	if (not _orientation_is_new and not _scaling_is_new) {
		if (not _position_is_new)
			return;
		
		// only the position was changed.
		for (uint8_t i = 0; i < 3; ++i)
			_model_mat[3][i] = static_cast<float>(_position[i]);
		_position_is_new = true;
		return;

	}

	_model_mat = glm::mat4(1.0);
	for (uint8_t i = 0; i < 3; ++i)
		_model_mat[3][i] = static_cast<float>(_position[i]);

	_model_mat = _model_mat * static_cast<glm::mat4>(glm::toMat4(_quat));
	_model_mat = glm::scale(_model_mat, _scaling);
	_orientation_is_new = false;
	_position_is_new = false;
	_scaling_is_new = false;
}
}