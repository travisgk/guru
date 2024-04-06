#include "material_list.hpp"
#include <iostream>

namespace gu {
namespace res {
MaterialList MaterialList::material_list;

std::shared_ptr<Material> MaterialList::create_and_load(
	const std::filesystem::path &diffuse_path
) {
	std::shared_ptr<Material> material = std::make_shared<Material>();
	material->load_textures(diffuse_path);
	add_entry(material);
	return material;
}

std::shared_ptr<Material> MaterialList::create_and_load(
	const std::filesystem::path *paths
) {
	std::shared_ptr<Material> material = std::make_shared<Material>();
	material->load_textures(paths);
	add_entry(material);
	return material;
}
} // namespace res
} // namespace gu