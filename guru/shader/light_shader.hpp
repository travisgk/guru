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
#include "../environment/lights.hpp"

// this empty namespace contains local structs used for constructing 
// classes that hold GL locations (IDs).
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

struct SpotLightIDs : public PointLightIDs {
	GLint direction_ID = -1;
	GLint inner_cutoff_ID = -1;
	GLint outer_cutoff_ID = -1;
};
} // blank namespace

namespace gu {
class LightShader : public ModelShader {
protected:
	const size_t N_DIR_LIGHTS = 1;
	const size_t N_POINT_LIGHTS = 1;
	const size_t N_SPOT_LIGHTS = 1;

	GLint _uni_model_mat_4fv_ID = -1; // model matrix
	GLint _uni_view_pos_3fv_ID = -1; // view position
	GLint _uni_ambient_color_3fv_ID = -1; // omnipresent color

	std::vector<DirLightIDs> _uni_dir_light_IDs; // IDs per each DirLight
	std::vector<PointLightIDs> _uni_point_light_IDs; // IDs per each PointLight
	std::vector<SpotLightIDs> _uni_spot_light_IDs; // IDs per each SpotLight
	
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

public:
	// sets the uniforms of a specified DirLight at <index> on the video card
	// with the given <dir_light>, whose attributes are marked
	// with having been modified.
	void update_GL_dir_light(
		const GLsizei &index, DirLight &dir_light
	);
	
	// sets the uniforms of a specified PointLight at <index> on the video card
	// with the given <point_light>, whose attributes are marked
	// with having been modified.
	void update_GL_point_light(
		const GLsizei &index, PointLight &point_light
	);

	// sets the uniforms of a specified SpotLight at <index> on the video card
	// with the given <spot_light>, whose attributes are marked
	// with having been modified.
	void update_GL_spot_light(
		const GLsizei &index, SpotLight &spot_light
	);

	inline void set_ambient_color(const glm::vec3 &vec) const {
		glUniform3fv(_uni_ambient_color_3fv_ID, 1, &vec[0]);
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

	inline void set_spot_light_dir(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_spot_light_IDs[index].direction_ID, 1, &vec[0]);
	}

	inline void set_spot_light_pos(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_spot_light_IDs[index].position_ID, 1, &vec[0]);
	}

	inline void set_spot_light_diffuse(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_spot_light_IDs[index].diffuse_ID, 1, &vec[0]);
	}

	inline void set_spot_light_specular(
		const GLsizei &index, const glm::vec3 &vec
	) const {
		glUniform3fv(_uni_spot_light_IDs[index].specular_ID, 1, &vec[0]);
	}

	inline void set_spot_light_constant(
		const GLsizei &index, const float &constant
	) const {
		glUniform1f(_uni_spot_light_IDs[index].constant_ID, constant);
	}

	inline void set_spot_light_linear(
		const GLsizei &index, const float &linear
	) const {
		glUniform1f(_uni_spot_light_IDs[index].linear_ID, linear);
	}

	inline void set_spot_light_quadratic(
		const GLsizei &index, const float &quadratic
	) const {
		glUniform1f(_uni_spot_light_IDs[index].quadratic_ID, quadratic);
	}

	inline void set_spot_light_inner_cutoff(
		const GLsizei &index, const float &inner_cutoff
	) const {
		glUniform1f(
			_uni_spot_light_IDs[index].inner_cutoff_ID, inner_cutoff
		);
	}

	inline void set_spot_light_outer_cutoff(
		const GLsizei &index, const float &outer_cutoff
	) const {
		glUniform1f(
			_uni_spot_light_IDs[index].outer_cutoff_ID, outer_cutoff
		);
	}
};
} // namespace gu