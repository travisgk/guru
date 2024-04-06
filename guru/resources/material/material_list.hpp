/**
 * material_list.hpp
 * ---
 * this file defines the <MaterialList> class, a child class of ResourceList
 * which will maintain a list of all the Materials loaded into Guru. 
 * it will also maintain how many sources are making use of the Material, 
 * and will delete the Material from memory 
 * once no more sources are making use of it.
 *
 */

#pragma once
#include "../resource_list.hpp"
#include "material.hpp"

namespace gu {
namespace res {
class MaterialList : public ResourceList<Material> {
public:
	static MaterialList material_list;

private:
	// no other instances of this class can be created.
	inline MaterialList() : ResourceList("Material") {}

public:
	// returns a created a shared pointer entry for a new Material
	// that has its textures loaded based on the given <diffuse_path>.
	std::shared_ptr<Material> create_and_load(
		const std::filesystem::path &diffuse_path
	);
	
	// returns a created a shared pointer entry for a new Material
	// that has its textures loaded from the given <paths>.
	std::shared_ptr<Material> create_and_load(
		const std::filesystem::path *paths
	);
};
}
}