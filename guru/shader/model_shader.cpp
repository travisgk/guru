#include "model_shader.hpp"
#include "../resources/material/material.hpp"
#include "../system/settings.hpp"

namespace gu {
void ModelShader::_config_uniform_IDs() {
	// finds IDs of uniform variables in the ModelShader.
	glLinkProgram(_program_ID);
	_uni_PVM_mat_4fv_ID = glGetUniformLocation(_program_ID, "_PVM_mat");
	_set_bone_mat_uniform_IDs();
	
	// gets the locations of the texture IDs.
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

void ModelShader::_set_bone_mat_uniform_IDs() {
	if (glGetUniformLocation(_program_ID, "_bone_mats[0]") == -1) {
		_uses_animation = false;
		return;
	}
	
	_uses_animation = true;
	_uni_bone_mat_4fv_IDs.reserve(Settings::MAX_BONES);
	for (size_t i = 0; i < Settings::MAX_BONES; ++i) {
		_uni_bone_mat_4fv_IDs[i] = glGetUniformLocation(
			_program_ID,
			("_bone_mats" + '[' + std::to_string(i) + ']').c_str()
		);
	}
}
} // namespace gu