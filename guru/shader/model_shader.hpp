/**
 * model_shader.hpp
 * ---
 * this file defines the ModelShader class as a child of the Shader class,
 * which is built to display a ModelResource.
 * 
 */

#pragma once
#include "shader.hpp"
#include <glm/mat4x4.hpp>

namespace gu {
class ModelShader : public Shader {
protected:
	GLint _uni_PVM_mat_4fv_ID = -1; // projection-view-model matrix
	std::vector<GLint> _uni_bone_mat_4fv_IDs; // animation bone matrices
	bool _uses_animation = false;

	// sets the class's contained uniform IDs by searching for them in the code.
	virtual void _config_uniform_IDs() override;

	// sets the uniform IDs for each element of "bone_mats" in the ModelShader.
	void _set_bone_mat_uniform_IDs();

public:
	// sets the projection-view-model matrix in the ModelShader.
	// this will be the "uniform mat4 _PVM_mat" in the vertex shader.
	inline void set_PVM_mat(const glm::mat4 &mat) const {
		glUniformMatrix4fv(_uni_PVM_mat_4fv_ID, 1, GL_FALSE, &mat[0][0]);
	}
};
} // namespace gu