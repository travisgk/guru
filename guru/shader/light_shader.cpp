#include "light_shader.hpp"
#include "../resources/material/material.hpp"
#include <iostream> // debug

static GLint get_array_index_ID(
	const GLuint &program_ID,
	const std::string &arr_name,
	const size_t &index,
	const std::string &suffix = ""
) {
	return glGetUniformLocation(
		program_ID,
		(arr_name + '[' + std::to_string(index) + ']' + suffix).c_str()
	);
}

// sets the uniform ID for the light direction variable in the shader.
template <typename T>
static void set_light_dir_ID(
	T &IDs_struct, 
	const GLuint &program_ID, 
	const std::string &arr_name,
	const size_t &index
) {
	IDs_struct.direction_ID = get_array_index_ID(program_ID, arr_name, index);
}

// sets the uniform ID for the light position variable in the shader.
template <typename T>
static void set_light_pos_ID(
	T &IDs_struct, 
	const GLuint &program_ID, 
	const std::string &arr_name,
	const size_t &index
) {
	IDs_struct.position_ID = get_array_index_ID(program_ID, arr_name, index);
}

// sets the uniform IDs for the light color variables in the shader.
template <typename T>
static void set_light_color_IDs(
	T &IDs_struct, 
	const GLuint &program_ID, 
	const std::string &arr_name,
	const size_t &i
) {
	static const std::string S[2] = {".diffuse", ".specular"};
	IDs_struct.diffuse_ID = get_array_index_ID(program_ID, arr_name, i, S[0]);
	IDs_struct.specular_ID = get_array_index_ID(program_ID, arr_name, i, S[1]);
}

// sets the uniform IDs for the light attenuation variables in the shader.
template <typename T>
static void set_light_atten_IDs(
	T &IDs_struct, 
	const GLuint &program_ID, 
	const std::string &arr_name,
	const size_t &i
) {
	static const std::string S[3] = {".constant", ".linear", ".quadratic"};
	IDs_struct.constant_ID = get_array_index_ID(program_ID, arr_name, i, S[0]);
	IDs_struct.linear_ID = get_array_index_ID(program_ID, arr_name, i, S[1]);
	IDs_struct.quadratic_ID = get_array_index_ID(program_ID, arr_name, i, S[2]);
}

template <typename I, typename T>
static void update_GL_light_position(const I &IDs, T &light) {
	if (light.position_needs_GL_update()) {
		const glm::vec3 &pos = static_cast<glm::vec3>(light.position());
		glUniform3fv(IDs.position_ID, 1, &pos[0]);
		light.set_as_entirely_GL_updated();
	}
}

template <typename I, typename T>
static void update_GL_light_direction(const I &IDs, T &light) {
	if (light.direction_needs_GL_update()) {
		const glm::vec3 &dir = static_cast<glm::vec3>(light.forward());
		glUniform3fv(IDs.direction_ID, 1, &dir[0]);
		light.set_direction_as_GL_updated();
	}
}

template <typename I, typename T>
static void update_GL_light_colors(const I &IDs, T &light) {
	if (light.diffuse().needs_GL_update()) {
		glUniform3fv(IDs.diffuse_ID, 1, &light.diffuse().rgb()[0]);
		light.diffuse().set_as_GL_updated();
	}

	if (light.specular().needs_GL_update()) {
		glUniform3fv(IDs.specular_ID, 1, &light.specular().rgb()[0]);
		light.specular().set_as_GL_updated();
	}
}

template <typename I, typename T>
static void update_GL_attenuation(const I &IDs, T &light) {
	if (not light.attenuation_needs_GL_update())
		return;

	if (light.constant().needs_GL_update()) {
		glUniform1f(IDs.constant_ID, light.constant().value());
		light.constant().set_as_GL_updated();
	}

	if (light.linear().needs_GL_update()) {
		glUniform1f(IDs.linear_ID, light.linear().value());
		light.linear().set_as_GL_updated();
	}

	if (light.quadratic().needs_GL_update()) {
		glUniform1f(IDs.quadratic_ID, light.quadratic().value());
		light.quadratic().set_as_GL_updated();
	}

	light.set_attenuation_as_GL_updated();
}

namespace gu {
void LightShader::_config_uniform_IDs() {
	// finds IDs of uniform variables in the LightShader.
	glLinkProgram(_program_ID);
	_uni_PVM_mat_4fv_ID = glGetUniformLocation(_program_ID, "_PVM_mat");
	_uni_model_mat_4fv_ID = glGetUniformLocation(_program_ID, "_model_mat");
	_uni_view_pos_3fv_ID = glGetUniformLocation(_program_ID, "_view_pos");
	_uni_ambient_color_3fv_ID = glGetUniformLocation(_program_ID, "_ambient_color");

	_uni_dir_light_IDs.resize(N_DIR_LIGHTS);
	for (size_t i = 0; i < N_DIR_LIGHTS; ++i) {
		DirLightIDs &IDs = _uni_dir_light_IDs[i];
		set_light_dir_ID(IDs, _program_ID, "_dir_light_dirs", i);
		set_light_color_IDs(IDs, _program_ID, "_dir_lights", i);
	}
	

	_uni_point_light_IDs.resize(N_POINT_LIGHTS);
	for (size_t i = 0; i < N_POINT_LIGHTS; ++i) {
		PointLightIDs &IDs = _uni_point_light_IDs[i];
		set_light_pos_ID(IDs, _program_ID, "_point_light_pos", i);
		set_light_color_IDs(IDs, _program_ID, "_point_lights", i);
		set_light_atten_IDs(IDs, _program_ID, "_point_lights", i);
	}

	_uni_spot_light_IDs.resize(N_SPOT_LIGHTS);
	for (size_t i = 0; i < N_SPOT_LIGHTS; ++i) {
		SpotLightIDs &IDs = _uni_spot_light_IDs[i];
		set_light_dir_ID(IDs, _program_ID, "_spot_light_dirs", i);
		set_light_pos_ID(IDs, _program_ID, "_spot_light_pos", i);
		set_light_color_IDs(IDs, _program_ID, "_spot_lights", i);
		set_light_atten_IDs(IDs, _program_ID, "_spot_lights", i);
		IDs.inner_cutoff_ID = get_array_index_ID(
			_program_ID, "_spot_lights", i, ".inner_cutoff"
		);
		IDs.outer_cutoff_ID = get_array_index_ID(
			_program_ID, "_spot_lights", i, ".outer_cutoff"
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
	set_ambient_color(glm::vec3(0.0f, 0.0f, 0.0f));
	for (uint8_t i = 0; i < Material::MAP_TYPE::ENUM_MAX; ++i)
		glUniform1i(uni_map_texture_1i_IDs[i], Material::MAP_TYPE::DIFFUSE + i);
	glUseProgram(0);
}

void LightShader::update_GL_dir_light(
	const GLsizei &index, DirLight &dir_light
) {
	if (
		index >= _uni_dir_light_IDs.size()
		and not dir_light.needs_any_GL_update()
	)
		return;

	update_GL_light_direction(_uni_dir_light_IDs[index], dir_light);
	update_GL_light_colors(_uni_dir_light_IDs[index], dir_light);
}

void LightShader::update_GL_point_light(
	const GLsizei &index, PointLight &point_light
) {
	if (
		index >= _uni_point_light_IDs.size()
		and not point_light.needs_any_GL_update()
	)
		return;

	update_GL_light_position(_uni_point_light_IDs[index], point_light);
	update_GL_light_colors(_uni_point_light_IDs[index], point_light);
	update_GL_attenuation(_uni_point_light_IDs[index], point_light);
	point_light.set_as_entirely_GL_updated();
}

void LightShader::update_GL_spot_light(
	const GLsizei &index, SpotLight &spot_light
) {
	if (
		index >= _uni_spot_light_IDs.size() 
		and not spot_light.needs_any_GL_update()
	)
		return;

	update_GL_light_position(_uni_spot_light_IDs[index], spot_light);
	update_GL_light_direction(_uni_spot_light_IDs[index], spot_light);
	update_GL_light_colors(_uni_spot_light_IDs[index], spot_light);
	update_GL_attenuation(_uni_spot_light_IDs[index], spot_light);

	if (spot_light.inner_cutoff().needs_GL_update()) {
		(index, spot_light.inner_cutoff().value());
		spot_light.inner_cutoff().set_as_GL_updated();
	}

	if (spot_light.outer_cutoff().needs_GL_update()) {
		(index, spot_light.outer_cutoff().value());
		spot_light.outer_cutoff().set_as_GL_updated();
	}
	spot_light.set_as_entirely_GL_updated();
}
} // namespace gu