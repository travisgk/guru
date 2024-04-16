/**
 * mesh.hpp
 * ---
 * this file defines the Vertex struct and Mesh class, 
 * with a Mesh being a unit of a Model,
 * which is composed of vertices, indices, and
 * a shared pointer to a Material.
 *
 */

#pragma once
#include <map>
#include <stdint.h>
#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <assimp/scene.h>
#include "../../system/settings.hpp"
#include "../material/material.hpp"

namespace {
/**
 * Vertex
 * ---
 * this local struct is used to buffer geometric information 
 * from a file to the video card.
 * 
 */
struct Vertex {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec2 uv = glm::vec2(0.0f);
	glm::vec3 normal = glm::vec3(0.0f);
	#if not defined(GURU_DISABLE_TANGENT_SPACE)
	glm::vec3 tangent = glm::vec3(0.0f);
	glm::vec3 bitangent = glm::vec3(0.0f);
	#endif
	int bone_IDs[gu::Settings::MAX_BONE_INFLUENCES];
	float weights[gu::Settings::MAX_BONE_INFLUENCES];

	inline Vertex() { set_bone_data_to_default(); }

	// sets all rigging information in the Vertex to not be used.
	void set_bone_data_to_default() {
		for (uint8_t i = 0; i < gu::Settings::MAX_BONE_INFLUENCES; ++i) {
			bone_IDs[i] = -1;
			weights[i] = 0.0f;
		}
	}

	// sets the next empty rigging information elements of <bone_IDs> and <weights>
	// to the given <bone_ID> and <weight>.
	void set_bone_data(int bone_ID, const float &weight) {
		for (uint8_t i = 0; i < gu::Settings::MAX_BONE_INFLUENCES; ++i) {
			if (bone_IDs[i] == -1) {
				// a bone ID is not yet set for this influence.
				bone_IDs[i] = bone_ID;
				weights[i] = weight;
				break;
			}
		}
	}
};
} // blank namespace

namespace gu {
class Mesh {
public:
	/**
	* Mesh::RigInfo
	* ---
	* this struct contains information used to organize bones
	* from a loaded 3D model.
	* 
	*/
	struct RigInfo {
		// index in Animator's <_final_bone_matrices>.
		int bone_ID = -1;

		// transforms from model space to bone space.
		glm::mat4 local_space_to_bone = glm::mat4(1.0f);
	};

	/**
	* Mesh::Override
	* ---
	* this struct specifies to a ModelResource's draw function
	* to override the bound Material for a particular <mesh_index>
	* in the ModelResource's Mesh list and change its default Material
	* to the given <material>.
	* 
	* this could perhaps be useful for rendering texture-based animation
	* that uses multiple different Materials.
	* 
	*/
	struct Override : public Material::Override {
		size_t mesh_index = 0;
		std::shared_ptr<Material> material = nullptr;

		inline Override(
			const size_t &mesh_index,
			const std::shared_ptr<Material> &material
		) : Material::Override(mesh_index, material) {}

		bool operator<(const Override &other) const {
			return mesh_index < other.mesh_index;
		}
	};

private:
	std::string _name = "";
	size_t _material_index = 0;
	GLuint _vao_ID = 0; // vertex array object
	GLuint _vbo_ID = 0; // vertex buffer object
	GLuint _ebo_ID = 0; // element buffer object
	GLsizei _n_indices = 0; // number of indices.

public:
	// dtor. deletes the allocated VAO, VBO, and EBO.
	~Mesh();

	// returns the name of the Mesh that was provided upon loading.
	inline const std::string &name() const { return _name; }

	// returns the index of the used Material shared pointer
	// in the ModelResource instance which contains this Mesh.
	inline const size_t &material_index() const { return _material_index; }

	// loads the bone information into the given map,
	// loads the vertices and indices into local vectors from the given aiMesh,
	// then creates the VAO, VBO, and EBO, 
	// and finally sends the data to the videocard.
	void load(
		std::map<std::string, Mesh::RigInfo> &rig_info_map,
		aiMesh *ai_mesh, 
		const aiScene *scene, 
		const std::filesystem::path &model_directory,
		const size_t &material_index
	);

private:
	// sets the Mesh's VAO, VBO, and EBO by 
	// sending the given <vertices> and <indices> vectors to the videocard.
	void _send_to_videocard(
		const std::vector<Vertex>& vertices, 
		const std::vector<uint32_t>& indices
	);

public:
	// binds and draws the Mesh's geometry with OpenGL.
	void draw() const;
};
} // namespace gu