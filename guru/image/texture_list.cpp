#include "texture_list.hpp"
#include <iostream>

static const std::filesystem::path NO_PATH_FOUND = std::filesystem::path("");

namespace gu {
namespace texture {
std::hash<std::string> TextureList::_hasher;
std::vector<size_t> TextureList::_times_used;
std::vector<GLuint> TextureList::_texture_IDs;
std::vector<std::filesystem::path> TextureList::_texture_paths;
std::unordered_map<size_t, size_t> TextureList::_hash_to_index;

GLuint TextureList::find_existing_texture_ID(
	const std::filesystem::path &path
) {
	size_t hash_num = _hasher(path.string());
	auto iter = _hash_to_index.find(hash_num);
	if (iter != _hash_to_index.end()) {
		++_times_used[iter->second];
		return _texture_IDs[iter->second];
	}
	return NO_TEXTURE;
}

const std::filesystem::path &TextureList::find_path_from_GL_ID(
	const GLuint &texture_ID
) {
	for (size_t i = 0; i < _texture_IDs.size(); ++i) {
		if (_texture_IDs[i] == texture_ID)
			return _texture_paths[i];
	}
	return NO_PATH_FOUND;
}

void TextureList::add_entry(
	const GLuint &texture_ID, const std::filesystem::path &path
) {
	size_t hash_num = _hasher(path.string());
	auto iter = _hash_to_index.find(hash_num);
	if (iter == _hash_to_index.end()) {
		size_t index = _texture_paths.size();
		_times_used.push_back(1);
		_texture_IDs.push_back(texture_ID);
		_texture_paths.push_back(path);
		_hash_to_index[hash_num] = index;
	}
}

void TextureList::delete_texture(const GLuint &texture_ID) {
	bool ID_found = false;
	size_t element_num;
	for (element_num = 0; element_num < _texture_IDs.size(); ++element_num) {
		if (_texture_IDs[element_num] == texture_ID) {
			ID_found = true;
			break;
		}
	}

	if (not ID_found or --_times_used[element_num] > 0)
		// a texture not found or still in use won't be deleted.
		return;

	const std::filesystem::path &path = _texture_paths[element_num];
	auto it_times_used = _times_used.begin() + element_num;
	auto it_ID = _texture_IDs.begin() + element_num;
	auto it_path = _texture_paths.begin() + element_num;
	size_t hash_num = _hasher(path.string());
	_times_used.erase(it_times_used);
	_texture_IDs.erase(it_ID);
	_texture_paths.erase(it_path);
	_hash_to_index.erase(hash_num);
	glDeleteTextures(1, &texture_ID);
}
} // namespace texture
} // namespace gu