/**
 * mesh.hpp
 * ---
 * this file defines the Vertex and Mesh class, 
 * with a Mesh being a unit of a Model.
 *
 */

#pragma once
#include <stdint.h>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <assimp/scene.h>
#include "material.hpp"

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
	std::shared_ptr<Material> _material;
	GLuint _vao_ID = 0; // vertex array object
	GLuint _vbo_ID = 0; // vertex buffer object
	GLuint _ebo_ID = 0; // element buffer object

public:
	void load(
		aiMesh *ai_mesh, 
		const aiScene *scene, 
		const std::filesystem::path &model_directory
	);

	// _material
	std::filesystem::path material_path() const;

private:
	void _send_to_videocard(
		std::vector<Vertex>& vertices, std::vector<uint32_t>& indices
	);
};
} // namespace gu