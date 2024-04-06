#include "skybox_shader.hpp"
#include "../resources/material/material.hpp"

namespace gu {
void SkyboxShader::_config_uniform_IDs() {
	// finds IDs of uniform variables in the SkyboxShader.
	glLinkProgram(_program_ID);
	_uni_PV_mat_4fv_ID = glGetUniformLocation(_program_ID, "_PV_mat");
	GLint uni_skybox_texture_1i_ID = glGetUniformLocation(
		_program_ID, "_skybox_texture_ID"
	);
	glLinkProgram(0);

	// the skybox texture is expected to be bound to 
	// the GL_TEXTUREx that corresponds to Material::MAP_TYPE::SKYBOX.
	use();
	glUniform1i(uni_skybox_texture_1i_ID, Material::MAP_TYPE::SKYBOX);
	glUseProgram(0);
}
} // namespace gu