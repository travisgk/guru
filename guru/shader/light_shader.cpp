#include "light_shader.hpp"
#include "../resources/material/material.hpp"
#include <iostream> // debug

namespace gu {
void LightShader::_config_uniform_IDs() {
	// finds IDs of uniform variables in the LightShader.
	glLinkProgram(_program_ID);
	_uni_PVM_mat_4fv_ID = glGetUniformLocation(_program_ID, "_PVM_mat");
	_uni_model_mat_4fv_ID = glGetUniformLocation(_program_ID, "_model_mat");
	_uni_view_pos_3fv_ID = glGetUniformLocation(_program_ID, "_view_pos");
	
	_uni_dir_light_IDs.resize(N_DIR_LIGHTS);
	for (size_t i = 0; i < N_DIR_LIGHTS; ++i) {
		_uni_dir_light_IDs[i].direction_ID = glGetUniformLocation(
			_program_ID,
			("_dir_light_dirs[" + std::to_string(i) + ']').c_str()
		);
		_uni_dir_light_IDs[i].diffuse_ID = glGetUniformLocation(
			_program_ID,
			("_dir_lights[" + std::to_string(i) + "].diffuse").c_str()
		);
		_uni_dir_light_IDs[i].specular_ID = glGetUniformLocation(
			_program_ID,
			("_dir_lights[" + std::to_string(i) + "].specular").c_str()
		);
	}

	_uni_point_light_IDs.resize(N_POINT_LIGHTS);
	for (size_t i = 0; i < N_POINT_LIGHTS; ++i) {
		_uni_point_light_IDs[i].position_ID = glGetUniformLocation(
			_program_ID, 
			("_point_light_pos[" + std::to_string(i) + ']').c_str()
		);
		_uni_point_light_IDs[i].diffuse_ID = glGetUniformLocation(
			_program_ID,
			("_point_lights[" + std::to_string(i) + "].diffuse").c_str()
		);
		_uni_point_light_IDs[i].specular_ID = glGetUniformLocation(
			_program_ID,
			("_point_lights[" + std::to_string(i) + "].specular").c_str()
		);
		_uni_point_light_IDs[i].constant_ID = glGetUniformLocation(
			_program_ID,
			("_point_lights[" + std::to_string(i) + "].constant").c_str()
		);
		_uni_point_light_IDs[i].linear_ID = glGetUniformLocation(
			_program_ID,
			("_point_lights[" + std::to_string(i) + "].linear").c_str()
		);
		_uni_point_light_IDs[i].quadratic_ID = glGetUniformLocation(
			_program_ID,
			("_point_lights[" + std::to_string(i) + "].quadratic").c_str()
		);
	}
	
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