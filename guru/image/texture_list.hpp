/**
 * texture_list.hpp
 * ---
* this file defines the <TextureList> struct, which will maintain a list
* of all the images loaded into Guru. it will also maintain how
* many sources are making use of a texture, and will delete the texture
* from memory only when no more sources are making use of it.
*
*/
#pragma once
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>
#include <glad/gl.h>
#include "color.hpp"

namespace gu {
namespace texture {
static const GLuint NO_TEXTURE = static_cast<GLuint>(0);

struct TextureList {
private:
	static std::hash<std::string> _hasher;
	static std::vector<size_t> _times_used;
	static std::vector<GLuint> _texture_IDs;
	static std::vector<std::filesystem::path> _texture_paths;
	static std::unordered_map<size_t, size_t> _hash_to_index;

	// instances of this struct cannot be created.
	inline TextureList() {}
public:
	static GLuint find_existing_texture_ID(
		const std::filesystem::path &path
	);

	static const std::filesystem::path &find_path_from_GL_ID(
		const GLuint &texture_ID
	);

	static void add_entry(
		const GLuint &texture_ID, const std::filesystem::path &path 
	);

	static void delete_texture(const GLuint &texture_ID);
};
}
}