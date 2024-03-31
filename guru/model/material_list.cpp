#include "material_list.hpp"
#include <iostream>

static const std::filesystem::path NO_PATH_FOUND = std::filesystem::path("");

namespace gu {
namespace material {
std::hash<std::string> MaterialList::_hasher;
std::vector<std::shared_ptr<Material>> MaterialList::_materials;
std::unordered_map<size_t, size_t> MaterialList::_hash_to_index;

const std::shared_ptr<Material> &MaterialList::find_existing_material(
	const std::filesystem::path &diffuse_path
) {
	size_t hash_num = _hasher(diffuse_path.string());
	auto iter = _hash_to_index.find(hash_num);
	if (iter != _hash_to_index.end()) {
		return _materials[iter->second];
	}
	return nullptr;
}

void MaterialList::add_entry(const std::shared_ptr<Material>& material) {
	size_t hash_num = _hasher(material->diffuse_path().string());
	auto iter = _hash_to_index.find(hash_num);
	if (iter == _hash_to_index.end()) {
		size_t index = _materials.size();
		_materials.push_back(material);
		_hash_to_index[hash_num] = index;
	}
}

void MaterialList::delete_material(
	const std::filesystem::path& diffuse_path
) {
	if (diffuse_path.empty())
		return;
	
	bool material_found = false;
	size_t element_num;
	for (element_num = 0; element_num < _materials.size(); ++element_num) {
		if (_materials[element_num]->diffuse_path() == diffuse_path) {
			material_found = true;
			break;
		}
	}

	if (not material_found or _materials[element_num].use_count() > 1)
		// a Material not found or still in use won't be deleted.
		return;

	auto it_material = _materials.begin() + element_num;
	size_t hash_num = _hasher(
		_materials[element_num]->diffuse_path().string()
	);
	_materials.erase(it_material);
	_hash_to_index.erase(hash_num);
}
} // namespace texture
} // namespace gu