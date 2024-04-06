/**
 * mesh.hpp
 * ---
 * this file defines the Vertex and Mesh class, 
 * with a Mesh being a unit of a Model,
 * which is composed of vertices, indices, and
 * a shared pointer to a Material.
 *
 */

#pragma once
#include <stdint.h>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <assimp/scene.h>
#include "../material/material.hpp"

namespace gu {
struct Vertex {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 uv = glm::vec2(0.0f, 0.0f);
	glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
	#if not defined(GURU_DISABLE_TANGENT_SPACE)
	glm::vec3 tangent = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
	#endif
};

class Mesh {
private:
	size_t _material_index = 0;
	GLuint _vao_ID = 0; // vertex array object
	GLuint _vbo_ID = 0; // vertex buffer object
	GLuint _ebo_ID = 0; // element buffer object
	GLsizei _n_indices = 0; // number of indices.

public:
	// dtor. deletes the allocated VAO, VBO, and EBO.
	~Mesh();

	// returns the index of the used Material shared pointer
	// in the ModelResource instance which contains this Mesh.
	inline const size_t &material_index() const { return _material_index; }

	// loads the vertices and indices into local vectors from the given aiMesh,
	// then creates the VAO, VBO, and EBO, 
	// and then sends the data to the videocard.
	void load(
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