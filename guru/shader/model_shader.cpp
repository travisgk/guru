#include "model_shader.hpp"
#include "../resources/material/material.hpp"

namespace gu {
void ModelShader::_config_uniform_IDs() {
	// finds IDs of uniform variables in the SkyboxShader.
	glLinkProgram(_program_ID);
	_uni_PVM_mat_4fv_ID = glGetUniformLocation(_program_ID, "_PVM_mat");
	
	GLint uni_map_texture_1i_IDs[Material::MAP_TYPE::ENUM_MAX]{};
	for (uint8_t i = 0; i < Material::MAP_TYPE::ENUM_MAX; ++i) {
		std::string var_str = '_' + Material::MAP_TYPE_STRS[i] + "_texture_ID";
		uni_map_texture_1i_IDs[i] = glGetUniformLocation(
			_program_ID, var_str.c_str()
		);
	}

	// each texture is expected to be bound to 
	// the GL_TEXTUREx that corresponds to its Material::MAP_TYPE.
	glLinkProgram(0);
	use();
	for (uint8_t i = 0; i < Material::MAP_TYPE::ENUM_MAX; ++i)
		glUniform1i(uni_map_texture_1i_IDs[i], Material::MAP_TYPE::DIFFUSE + i);
	glUseProgram(0);
}
} // namespace gu