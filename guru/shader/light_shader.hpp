/**
 * light_shader.hpp
 * ---
 * this file defines the LightShader class as a child of the ModelShader class,
 * which is built to display a ModelResource with light calculations.
 * 
 */

#pragma once
#include "model_shader.hpp"
#include <vector>
#include "../resources/color.hpp"

namespace {
struct LightColorIDs {
	GLint diffuse_ID = -1;
	GLint specular_ID = -1;

protected:
	inline LightColorIDs() {}
};

struct AttenuationIDs {
	GLint constant_ID = -1;
	GLint linear_ID = -1;
	GLint quadratic_ID = -1;

protected:
	inline AttenuationIDs() {}
};

struct DirLightIDs : public LightColorIDs {
	GLint direction_ID = -1;
};

struct PointLightIDs : public LightColorIDs, public AttenuationIDs {
	GLint position_ID = -1;
};
};

namespace gu {
class LightShader : public ModelShader {
protected:
	const size_t N_DIR_LIGHTS = 1;
	const size_t N_POINT_LIGHTS = 1;

	GLint _uni_model_mat_4fv_ID = -1; // model matrix
	GLint _uni_view_pos_3fv_ID = -1; // view position

	std::vector<DirLightIDs> _uni_dir_light_IDs;
	std::vector<PointLightIDs> _uni_point_light_IDs;
	
	// sets the class's contained uniform IDs by searching for them in the code.
	virtual void _config_uniform_IDs() override;

public:
	// sets the model matrix in the LightShader.
	// this will be the "uniform mat4 _model_mat" in the vertex shader.
	inline void set_model_mat(const glm::mat4 &mat) const {
		glUniformMatrix4fv(_uni_model_mat_4fv_ID, 1, GL_FALSE, &mat[0][0]);
	}

	// sets the view position in the LightShader.
	// this will be the "uniform vec3 _view_pos" in the vertex shader.
	inline void set_view_pos(const glm::vec3 &vec) const {
		glUniform3fv(_uni_view_pos_3fv_ID, 1, &vec[0]);
	}

	inline void set_dir_light_dir(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_dir_light_IDs[index].direction_ID, 1, &vec[0]);
	}

	inline void set_dir_light_diffuse(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_dir_light_IDs[index].diffuse_ID, 1, &vec[0]);
	}

	inline void set_dir_light_specular(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_dir_light_IDs[index].specular_ID, 1, &vec[0]);
	}

	inline void set_point_light_pos(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_point_light_IDs[index].position_ID, 1, &vec[0]);
	}

	inline void set_point_light_diffuse(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_point_light_IDs[index].diffuse_ID, 1, &vec[0]);
	}

	inline void set_point_light_specular(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_point_light_IDs[index].specular_ID, 1, &vec[0]);
	}

	inline void set_point_light_constant(
		const GLsizei &index, const float &constant
	) const {
		glUniform1f(_uni_point_light_IDs[index].constant_ID, constant);
	}

	inline void set_point_light_linear(
		const GLsizei &index, const float &linear
	) const {
		glUniform1f(_uni_point_light_IDs[index].linear_ID, linear);
	}

	inline void set_point_light_quadratic(
		const GLsizei &index, const float &quadratic
	) const {
		glUniform1f(_uni_point_light_IDs[index].quadratic_ID, quadratic);
	}
};
} // namespace gu