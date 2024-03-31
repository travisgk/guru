/**
 * material_list.hpp
 * ---
* this file defines the <MaterialList> struct, which will maintain a list
* of all the Materials loaded into Guru. it will also maintain how
* many sources are making use of the Material, and will delete the Material
* from memory only when no more sources are making use of it.
*
*/
#pragma once
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>
#include "material.hpp"

namespace gu {
namespace material {
struct MaterialList {
private:
	static std::hash<std::string> _hasher;
	static std::vector<std::shared_ptr<Material>> _materials;
	static std::unordered_map<size_t, size_t> _hash_to_index;

	// instances of this struct cannot be created.
	inline MaterialList() {}
public:
	static const std::shared_ptr<Material> &find_existing_material(
		const std::filesystem::path &diffuse_path
	);

	static void add_entry(const std::shared_ptr<Material> &material);

	static void delete_material(const std::filesystem::path& diffuse_path);
};
}
}