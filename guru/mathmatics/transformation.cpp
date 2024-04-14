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
	// _update_relative_directions();
	_update_model_matrix();
}

void Transformation::_update_model_matrix() {
	if (not _orientation_is_new and not _scaling_is_new) {
		if (not _position_is_new)
			return;
		
		// only the position was changed.
		for (uint8_t i = 0; i < 3; ++i)
			_model_mat[3][i] = static_cast<float>(_position[i]);
		_position_is_new = false;
		return;
	}

	// translates (places).
	_model_mat = glm::mat4(1.0);
	for (uint8_t i = 0; i < 3; ++i)
		_model_mat[3][i] = static_cast<float>(_position[i]);

	// rotates.
	const glm::dmat4 rot_mat = glm::toMat4(_quat);
	for (uint8_t i = 0; i < 3; ++i)
		for (uint8_t j = 0; j < 3; ++j)
			_model_mat[i][j] = static_cast<float>(rot_mat[i][j]);

	// scales.
	for (uint8_t i = 0; i < 3; ++i)
		for (uint8_t j = 0; j < 3; ++j)
			_model_mat[i][j] = _model_mat[i][j] * _scaling[j];

	// updates relative directions.
	if (_orientation_is_new) {
		for (uint8_t i = 0; i < 3; ++i) {
			_right[i] = rot_mat[0][i];
			_up[i] = rot_mat[1][i];
			_forward[i] = rot_mat[2][i];
		}
		#if defined(GURU_USE_LEFT_HANDED_COORDINATES)
		_right = glm::normalize(_right);
		#else
		_right = glm::normalize(-_right);
		#endif
		_up = glm::normalize(_up);
		_forward = glm::normalize(_forward);
	}
	_orientation_is_new = false;
	_position_is_new = false;
	_scaling_is_new = false;
}
} // namespace gu