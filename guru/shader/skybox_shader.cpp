#include "skybox_shader.hpp"

namespace gu {
void SkyboxShader::_config_uniform_IDs() {
	glLinkProgram(_program_ID);
	_uni_projview_mat_ID = glGetUniformLocation(_program_ID, "_projview_mat");
	GLint uni_skybox_texture_ID = glGetUniformLocation(
		_program_ID, "_skybox_texture_ID"
	);
	glLinkProgram(0);
	use();
	glUniform1i(uni_skybox_texture_ID, 0);
	glUseProgram(0);
}
} // namespace gu