#include "camera.hpp"
#include <algorithm>
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

namespace gu {
void Camera::set_field_of_view(const float& radians) {
	_fov = glm::radians(radians);
	_proj_mat_needs_update = true;
}

void Camera::set_min_render_distance(const float& distance) {
	_min_render_dist = distance;
	_proj_mat_needs_update = true;
}

void Camera::set_max_render_distance(const float& distance) {
	_max_render_dist = distance;
	_proj_mat_needs_update = true;
}

void Camera::framebuffer_size_callback(int width, int height) {
	height = std::max(1, height);
	float f_render_w = width * _render_section.z;
	float f_render_h = height * _render_section.w;
	_render_x = static_cast<GLint>(_render_section.x * f_render_w);
	_render_y = static_cast<GLint>(
		(1.0f - _render_section.y - _render_section.w) * f_render_h	
	);
	_render_w = static_cast<GLsizei>(f_render_w);
	_render_h = static_cast<GLsizei>(f_render_h);
	_render_ratio = abs(f_render_w / f_render_h);
	_proj_mat_needs_update = true;
}

void Camera::update() {
	_update_relative_directions();
	bool projview_needs_update = false;
	if (_orientation_is_new or _position_is_new) {
		_view_mat = glm::lookAt(_position, _position + _forward, _up);
		_orientation_is_new = false;
		_position_is_new = false;
		projview_needs_update = _orientation_is_new;
	}

	if (_proj_mat_needs_update) {
		if (_orthographic) {
			_proj_mat = glm::ortho(
				-1.0f / _render_ratio, 
				1.0f / _render_ratio,
				-1.0f * _render_ratio,
				_render_ratio,
				_min_render_dist,
				_max_render_dist
			);
		} else {
			_proj_mat = glm::perspective(
				_fov,
				_render_ratio,
				_min_render_dist,
				_max_render_dist
			);
		}
		_proj_mat_needs_update = false;
		projview_needs_update = true;
	}

	if (projview_needs_update)
		_projview_mat = _proj_mat * glm::mat4(glm::mat3(_view_mat));
}

void Camera::_set_ortho_projection(bool orthographic) {
	_orthographic = orthographic;
	_proj_mat_needs_update = true;
}
} // namespace gu