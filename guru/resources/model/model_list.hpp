/**
 * model_list.hpp
 * ---
 * this file defines the <ModelResourceList> class, a child of ResourceList,
 * which maintains all the ModelResources loaded into Guru. 
 * it will also maintain how many sources are making use of the ModelResource, 
 * and will delete the ModelResource from memory 
 * once no more sources are making use of it.
 *
 */

#pragma once
#include "model_resource.hpp"
#include "../resource_list.hpp"

namespace gu {
namespace res {
class ModelResourceList : public ResourceList<ModelResource> {
public:
	static ModelResourceList model_res_list;

private:
	// no other instances of this class can be created.
	inline ModelResourceList() : ResourceList("ModelResource") {}
};
}
}