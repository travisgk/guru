#include "mesh.hpp"
#include <iostream>
#include "material_list.hpp"

namespace gu {
static void load_mesh(
	std::vector<Vertex> &vertices,
	std::vector<uint32_t> &indices,
	std::shared_ptr<Material> &material,
	aiMesh *ai_mesh,
	const aiScene *scene,
	const std::filesystem::path &model_directory
);

void Mesh::load(
	aiMesh *ai_mesh, 
	const aiScene *scene, 
	const std::filesystem::path &model_directory
) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	_material = std::make_shared<Material>();
	load_mesh(vertices, indices, _material, ai_mesh, scene, model_directory);
	_send_to_videocard(vertices, indices);
}

std::filesystem::path Mesh::material_path() const {
	return _material ? _material->diffuse_path() : "";
}

static void set_vec2(glm::vec2& vec, const aiVector3D& value) {
	vec.x = value.x;
	vec.y = value.y;
}

static void set_vec3(glm::vec3& vec, const aiVector3D& value) {
	vec.x = value.x;
	vec.y = value.y;
	vec.z = value.z;
}

// loads the data contained in <ai_mesh> 
// to the given <vertices> and <indices> vectors.
void load_mesh(
	std::vector<Vertex> &vertices,
	std::vector<uint32_t> &indices,
	std::shared_ptr<Material> &material,
	aiMesh *ai_mesh, 
	const aiScene *scene, 
	const std::filesystem::path& model_directory
) {
	for (uint32_t i = 0; i < ai_mesh->mNumVertices; ++i) {
		vertices.push_back(Vertex());
		Vertex &vertex = vertices[vertices.size() - 1];
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

	for (uint32_t i = 0; i < ai_mesh->mNumFaces; ++i) {
		aiFace &face = ai_mesh->mFaces[i];
		for (uint32_t j=0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	aiMaterial *ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];
	if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) == 0)
		return;

	aiString ai_image_names[Material::N_MAP_TYPES];
	ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_image_names[0]);

	// ends function early if an existing material is found. 
	std::filesystem::path paths[Material::N_MAP_TYPES];
	paths[0] = model_directory / ai_image_names[0].C_Str();
	std::shared_ptr<Material> existing_material = (
		material::MaterialList::find_existing_material(paths[0])
	);
	if (existing_material) {
		material = existing_material;
		return;
	}

	#if not defined(GURU_DISABLE_TANGENT_SPACE)
	ai_material->GetTexture(aiTextureType_NORMALS, 0, &ai_image_names[1]);
	#endif
	ai_material->GetTexture(aiTextureType_SPECULAR, 0, &ai_image_names[2]);
	ai_material->GetTexture(aiTextureType_SHININESS, 0, &ai_image_names[3]);
	#if not defined(GURU_DISABLE_TANGENT_SPACE)
	ai_material->GetTexture(aiTextureType_DISPLACEMENT, 0, &ai_image_names[4]);
	#endif
	ai_material->GetTexture(aiTextureType_EMISSIVE, 0, &ai_image_names[5]);
	
	for (uint8_t i = 1; i < Material::N_MAP_TYPES; ++i) {
		if (ai_image_names[i].length > 0)
			paths[i] = model_directory / ai_image_names[i].C_Str();
	}
	std::cout << "starting loading material." << std::endl; // debug
	material->load_textures(paths);
	material::MaterialList::add_entry(material);
}

void Mesh::_send_to_videocard(
	std::vector<Vertex> &vertices, std::vector<uint32_t> &indices
) {
	glGenVertexArrays(1, &_vao_ID);
	glGenBuffers(1, &_vbo_ID);
	glGenBuffers(1, &_ebo_ID);
	glBindVertexArray(_vao_ID);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo_ID);
	GLsizeiptr verts_size = vertices.size() * sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, verts_size, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo_ID);
	GLsizeiptr inds_size = indices.size() * sizeof(uint32_t);
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
} // namespace gu