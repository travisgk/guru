#include "texture_list.hpp"
#include <iostream>
#include "../system/settings.hpp"

static const std::filesystem::path NO_PATH_FOUND = std::filesystem::path("");

namespace gu {
namespace texture {
std::hash<std::string> TextureList::_hasher;
std::vector<GLuint> TextureList::_texture_IDs;
std::vector<std::filesystem::path> TextureList::_texture_paths;
std::unordered_map<size_t, size_t> TextureList::_hash_to_index;

GLuint TextureList::find_existing_texture_ID(
	const std::filesystem::path& path
) {
	size_t hash_num = _hasher(path.string());
	auto iter = _hash_to_index.find(hash_num);
	if (iter != _hash_to_index.end())
		return _texture_IDs[iter->second];
	return NO_TEXTURE;
}

const std::filesystem::path& TextureList::find_path_from_GL_ID(
	const GLuint& texture_ID
) {
	for (size_t i = 0; i < _texture_IDs.size(); ++i) {
		if (_texture_IDs[i] == texture_ID)
			return _texture_paths[i];
	}
	return NO_PATH_FOUND;
}

void TextureList::add_entry(
	const GLuint& texture_ID, const std::filesystem::path& path
) {
	size_t hash_num = _hasher(path.string());
	auto iter = _hash_to_index.find(hash_num);
	if (iter == _hash_to_index.end()) {
		size_t index = _texture_paths.size();
		_texture_IDs.push_back(texture_ID);
		_texture_paths.push_back(path);
		_hash_to_index[hash_num] = index;
	}
}
} // namespace texture
} // namespace gu