/**
 * transformation.hpp
 * ---
 * this file defines the Transformation class, which represents
 * a point in 3D space with rotation and scaling.
 * 
 */

#pragma once
#include "quat_point.hpp"

namespace gu {
class Transformation : public QuatPoint {
protected:
	glm::mat4 _model_mat = glm::mat4(1.0);
	glm::vec3 _scaling = glm::vec3(1.0);
	bool _scaling_is_new = true;

public:
	// ctor. the function <_update_relative_directions()> 
	// will not set <_orientation_is_new> to false.
	inline Transformation() : QuatPoint(false) {}
	inline const glm::mat4 &get_model_matrix() const { 
		return _model_mat;
	}
	inline const glm::vec3 &get_scaling() const { return _scaling; }
	
	// sets consistent scaling across all three axes.
	void set_scaling(const float &absolute_scale);
	
	// updates the object's matrices. 
	// if the compiler flag GURU_AUTO_UPDATE_MATH_OBJECTS is not used,
	// this should be called on every frame, 
	// or whenever the object's attributes were modified.
	virtual void update() override;

protected:
	void _update_model_matrix();
};
}