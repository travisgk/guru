#include "model_list.hpp"
#include <iostream>

namespace gu {
namespace res {
ModelResourceList ModelResourceList::model_res_list;

std::shared_ptr<ModelResource> ModelResourceList::create_and_load(
	const std::filesystem::path &model_path
) {
	auto model_res = std::make_shared<ModelResource>();
	model_res->load(model_path);
	add_entry(model_res);
	return model_res;
}
} // namespace res
} // namespace gu