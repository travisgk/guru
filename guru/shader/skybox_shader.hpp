/**
 * skybox_shader.hpp
 * ---
 * this file defines the SkyboxShader class as a child of the Shader class,
 * which is built to display a cubemap texture as a skybox.
 * 
 */

#pragma once
#include "shader.hpp"
#include <glm/mat4x4.hpp>

namespace gu {
class SkyboxShader : public Shader {
protected:
	GLint _uni_PV_mat_4fv_ID = -1; // projection-view matrix

	// sets the class's contained uniform IDs by searching for them in the code.
	virtual void _config_uniform_IDs() override;

public:
	// sets the projection-view matrix in the SkyboxShader.
	// this will be the "uniform mat4 _PV_mat" in the vertex shader.
	inline void set_PV_mat_4fv(const glm::mat4 &mat) const {
		glUniformMatrix4fv(_uni_PV_mat_4fv_ID, 1, GL_FALSE, &mat[0][0]);
	}
};
} // namespace gu