#include "mesh.hpp"
#include <cctype>
#include <iostream>
#include "assimp_to_glm.hpp"
#include "../material/material_list.hpp"

static auto &material_list = gu::res::MaterialList::material_list;

namespace gu {
static void extract_bone_weights_for_verts(
	std::map<std::string, Mesh::BoneInfo> &bone_info_map,
	std::vector<Vertex> &vertices,
	aiMesh *ai_mesh,
	const aiScene *scene
) {
	for (size_t bone_index = 0; bone_index < ai_mesh->mNumBones; ++bone_index) {
		int bone_ID = -1;
		std::string bone_name = ai_mesh->mBones[bone_index]->mName.C_Str();
		if (bone_info_map.find(bone_name) == bone_info_map.end()) {
			Mesh::BoneInfo info;
			bone_ID = static_cast<int>(bone_info_map.size());
			info.id = bone_ID;
			set_mat4(info.offset, ai_mesh->mBones[bone_index]->mOffsetMatrix);
			bone_info_map[bone_name] = info;
		} else {
			bone_ID = bone_info_map[bone_name].id;
		}
		assert(bone_ID != -1);
		
		auto weights = ai_mesh->mBones[bone_index]->mWeights;
		int n_weights = ai_mesh->mBones[bone_index]->mNumWeights;
		for (int weight_index = 0; weight_index < n_weights; ++weight_index) {
			const int &vertex_ID = weights[weight_index].mVertexId;
			const float &weight = weights[weight_index].mWeight;
			assert(vertex_ID <= vertices.size());
			vertices[vertex_ID].set_bone_data(bone_ID, weight);
		}
	}
}

// static function which loads the data contained in <ai_mesh> 
// to the given <vertices> and <indices> vectors,
// and it sets the given <material> shared pointer by reference.
static void load_mesh(
	std::map<std::string, Mesh::BoneInfo> &bone_info_map,
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

	// loads rigging information.
	extract_bone_weights_for_verts(bone_info_map, vertices, ai_mesh, scene);

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
	std::map<std::string, Mesh::BoneInfo> &bone_map,
	aiMesh *ai_mesh, 
	const aiScene *scene, 
	const std::filesystem::path &model_directory,
	const size_t &material_index
) {
	_name = ai_mesh->mName.C_Str();
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	load_mesh(
		bone_map,
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

	ptr = (void*)offsetof(Vertex, bone_IDs);
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(
		5, Settings::MAX_BONE_INFLUENCES, GL_INT, sizeof(Vertex), ptr
	);

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(
		6, 
		Settings::MAX_BONE_INFLUENCES, 
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(Vertex), 
		(void *)offsetof(Vertex, weights)
	);

	glBindVertexArray(0);
}

void Mesh::draw() const {
	glBindVertexArray(_vao_ID);
	glDrawElements(GL_TRIANGLES, _n_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
} // namespace gu