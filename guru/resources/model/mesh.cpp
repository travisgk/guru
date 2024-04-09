#include "mesh.hpp"
#include <cctype>
#include <iostream>
#include "../material/material_list.hpp"

static auto &material_list = gu::res::MaterialList::material_list;

namespace gu {
// sets the given vec2 by reference from the given aiVector3D.
static void set_vec2(glm::vec2 &vec, const aiVector3D &value) {
	vec.x = value.x;
	vec.y = value.y;
}

// sets the given vec3 by reference from the given aiVector3D.
static void set_vec3(glm::vec3 &vec, const aiVector3D &value) {
	vec.x = value.x;
	vec.y = value.y;
	vec.z = value.z;
}

// static function which loads the data contained in <ai_mesh> 
// to the given <vertices> and <indices> vectors,
// and it sets the given <material> shared pointer by reference.
static void load_mesh(
	std::vector<Vertex> &vertices,
	std::vector<uint32_t> &indices,
	aiMesh *ai_mesh,
	const aiScene *scene,
	const std::filesystem::path &model_directory
) {
	// loads vertices.
	for (uint32_t i = 0; i < ai_mesh->mNumVertices; ++i) {
		vertices.push_back(Vertex());
		Vertex& vertex = vertices.back();
		set_vec3(vertex.position, ai_mesh->mVertices[i]);

		if (ai_mesh->mTextureCoords[0])
			set_vec2(vertex.uv, ai_mesh->mTextureCoords[0][i]);

		if (ai_mesh->HasNormals())
			set_vec3(vertex.normal, ai_mesh->mNormals[i]);

		#if not defined(GURU_DISABLE_TANGENT_SPACE)
		if (ai_mesh->HasTangentsAndBitangents()) {
			set_vec3(vertex.tangent, ai_mesh->mTangents[i]);
			set_vec3(vertex.bitangent, ai_mesh->mBitangents[i]);
		}
		#endif
	}

	// loads indices.
	for (uint32_t i = 0; i < ai_mesh->mNumFaces; ++i) {
		aiFace& face = ai_mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &_vao_ID);
	glDeleteBuffers(1, &_vbo_ID);
	glDeleteBuffers(1, &_ebo_ID);
}

void Mesh::load(
	aiMesh *ai_mesh, 
	const aiScene *scene, 
	const std::filesystem::path &model_directory,
	const size_t &material_index
) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	load_mesh(
		vertices, 
		indices, 
		ai_mesh, 
		scene, 
		model_directory
	);
	_n_indices = static_cast<GLsizei>(indices.size());
	_material_index = material_index;
	_send_to_videocard(vertices, indices);
}

void Mesh::_send_to_videocard(
	const std::vector<Vertex> &vertices, 
	const std::vector<uint32_t> &indices
) {
	glGenVertexArrays(1, &_vao_ID);
	glGenBuffers(1, &_vbo_ID);
	glGenBuffers(1, &_ebo_ID);
	glBindVertexArray(_vao_ID);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo_ID);
	GLsizeiptr verts_size = vertices.size() * sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, verts_size, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo_ID);
	GLsizeiptr inds_size = _n_indices * sizeof(uint32_t);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, inds_size, &indices[0], GL_STATIC_DRAW
	);

	// specifies how OpenGL should interpret the vertex data.
	void* ptr = (void*)0;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ptr);

	ptr = (void*)offsetof(Vertex, uv);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ptr);

	ptr = (void*)offsetof(Vertex, normal);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ptr);

	#if not defined(GURU_DISABLE_TANGENT_SPACE)
	ptr = (void*)offsetof(Vertex, tangent);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ptr);

	ptr = (void*)offsetof(Vertex, bitangent);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ptr);
	#endif

	glBindVertexArray(0);
}

void Mesh::draw() const {
	glBindVertexArray(_vao_ID);
	glDrawElements(GL_TRIANGLES, _n_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
} // namespace gu