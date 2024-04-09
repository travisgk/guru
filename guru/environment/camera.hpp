/**
 * camera.hpp
 * ---
 * this file defines the Camera class, whose matrices 
 * are used to transform the rendered objects 
 * to apply a particular perspective to a scene.
 *
 */

#pragma once
#include <glad/gl.h>
#include "../mathmatics/quat_point.hpp"

namespace gu {
class Camera : public QuatPoint {
protected:
	glm::mat4 _view_mat = glm::mat4(1.0); // view matrix
	glm::mat4 _proj_mat = glm::mat4(1.0); // projection matrix
	bool _proj_mat_needs_update = true; // update condition
	glm::mat4 _projview_mat = glm::mat4(1.0); // projection * view matrix
	glm::mat4 _skybox_mat = glm::mat4(1.0); // projview without position
	glm::vec4 _render_section = glm::vec4(0.0, 0.0, 1.0, 1.0);
	GLint _render_x = 0; // glViewport X
	GLint _render_y = 0; // glViewport Y
	GLsizei _render_w = 640; // glViewport width
	GLsizei _render_h = 480; // glViewport height
	float _render_ratio = 4.0f / 3.0f; // render width / render height
	bool _orthographic = false; // projection setting
	float _fov = glm::radians(35.0f); // field of view in radians
	float _min_render_dist = 0.01f; // minimum render distance
	float _max_render_dist = 1000.0f; // maximum render distance

public:
	// ctor. sets the position.
	Camera(const glm::dvec3 &position = glm::dvec3(0.0, 0.0, 0.0));

	// returns the view matrix.
	inline const glm::mat4 &get_view() const { return _view_mat; }

	// returns the projection matrix.
	inline const glm::mat4 &get_projection() const { return _proj_mat; }

	// returns the projection * view matrix.
	inline const glm::mat4 &get_projview() const { return _projview_mat; }

	// returns the projection * view matrix, 
	// with the view having the Camera's position stripped.
	inline const glm::mat4 &get_skybox_mat() const { return _skybox_mat; }

	// returns the Camera's field of view in radians.
	inline const float &get_field_of_view() const { return _fov; }

	// sets the projection matrix to use perspective projection.
	inline void use_perspective_projection() { 
		_set_ortho_projection(false); 
	}

	// sets the projection matrix to use orthographic projection.
	inline void use_orthographic_projection() { 
		_set_ortho_projection(true); 
	}
	
	// sets the GLviewport so that only the window fraction is rendered to.
	inline void use() {
		glViewport(_render_x, _render_y, _render_w, _render_h);
	}
	
	// sets the fraction of the screen that the Camera will render to.
	inline void set_render_section(const glm::vec4 &render_section) {
		_render_section = render_section;
	}

	void set_field_of_view(const float &radians);
	void set_min_render_distance(const float &distance);
	void set_max_render_distance(const float &distance);

private:
	// this is called by any member method that modifies the Cam's projection.
	void _set_camera_as_modified();
public:
	// updates how the Camera will utilize glViewport(...).
	// this should be called when the Window changes size.
	void framebuffer_size_callback(int width, int height);
	
	// updates the Camera's matrices.
	// if the compiler flag GURU_AUTO_UPDATE_MATH_OBJECTS is not used,
	// then this should be called on every frame, 
	// or whenever the object's attributes were modified.
	virtual void update() override;

protected:
	// sets the type of projection to be used.
	void _set_ortho_projection(bool orthographic);
};
} // namespace gu