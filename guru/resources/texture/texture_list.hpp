/**
 * texture_list.hpp
 * ---
* this file defines the <TextureList> class, a child of the ResourceList class,
* which will maintain a list of all the images loaded into Guru.
* it will also maintain how many sources are making use of a texture,
* and will delete the texture from memory
* once no more sources are making use of it.
*
*/

#pragma once
#include "texture_info.hpp"
#include "../color.hpp"
#include "../resource_list.hpp"

namespace gu {
namespace res {
class TextureList : public ResourceList<TextureInfo> {
public:
	static TextureList texture_list;

private:
	// no other instances of this class can be created.
	inline TextureList() : ResourceList("TextureInfo") {}

protected:
	// overrides this method in order to properly remove the OpenGL texture.
	inline virtual void _delete_resource(
		const std::shared_ptr<TextureInfo> &res_ptr
	) override {
		glDeleteTextures(1, &(res_ptr->texture_ID));
	}
};
} // namespace res
} // namespace gu