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
#include "../framebuffer/screenbuffer.hpp"

namespace gu {
class SkyboxShader : public Shader {
protected:
	GLint _uni_projview_mat_ID = 0;

	// sets the class's contained uniform IDs by searching for them in the code.
	virtual void _config_uniform_IDs() override;

public:
	inline void set_projview_mat(const glm::mat4 &mat) {
		glUniformMatrix4fv(_uni_projview_mat_ID, 1, GL_FALSE, &mat[0][0]);
	}
};
} // namespace gu