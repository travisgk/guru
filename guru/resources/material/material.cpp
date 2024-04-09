#include "material.hpp"
#include "../texture/load_texture.hpp"
#include "../texture/color_texture.hpp"
#include "../texture/texture_list.hpp"

namespace gu {
const Color Material::DEFAULT_COLORS[N_MAP_TYPES] = {
	Color(0.5f, 0.5f, 0.5f), // diffuse
	Color(0.5f, 0.5f, 1.0f), // normal 
	Color(0.5f, 0.5f, 0.5f), // displacement
	Color(1.0f, 1.0f, 1.0f), // roughness
	Color(0.0f, 0.0f, 0.0f), // ambient occlusion
	Color(0.0f, 0.0f, 0.0f), // metallic
	Color(0.0f, 0.0f, 0.0f)  // emissive
};

const std::string Material::MAP_TYPE_STRS[MAP_TYPE::ENUM_MAX] = {
	"diffuse",
	"normal",
	"displacement",
	"roughness",
	"ao",
	"metallic",
	"emissive",
	"skybox"
};

Material::Material() {
	_map_loaded[0] = true; // diffuse is always used
	for (uint8_t i = 1; i < N_MAP_TYPES; ++i)
		_map_loaded[i] = false;
}

Material::~Material() {
	// properly deletes every TextureInfo shared pointer.
	for (uint8_t i = 0; i < N_MAP_TYPES; ++i) {
		std::filesystem::path texture_path = _texture_infos[i]->path();
		_texture_infos[i] = nullptr;
		res::TextureList::texture_list.delete_entry(texture_path);
	}
}

// loads textures based on the given diffuse image path.
void Material::load_textures(const std::filesystem::path &diffuse_path) {
	std::filesystem::path paths[N_MAP_TYPES] = {diffuse_path};
	for (uint8_t i = 1; i < N_MAP_TYPES; ++i)
		paths[i] = "";
	load_textures(paths);
}

void Material::load_textures(const std::filesystem::path *paths) {
	// diffuse will always be used by a Shader,
	// and only the diffuse texture is used to determine transparency.
	_texture_infos[0] = res::load_texture(paths[0], &_transparent);
	_map_loaded[0] = true;

	// loads each texture map.
	std::string file_stem_str = (
		paths[0].parent_path().string() / paths[0].stem()
	).string();
	std::string file_extension_str = paths[0].extension().string();
	for (int i = 1; i < N_MAP_TYPES; ++i) {
		if (not paths[i].empty() and std::filesystem::exists(paths[i])) {
			_texture_infos[i] = res::load_texture(paths[i]);
		} else {
			// if the image path is not found,
			// then the program tries to find an image with the name
			// as the diffuse texture, but with the map type
			// name appended to it.
			// different extension types are searched.
			static const std::string EXTENSIONS[] = {
				".png", ".jpeg", ".jpg", ".bmp", ".gif"
			};
			static const int N_EXTENSIONS = (
				sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0])
			);
			bool image_found = false;
			for (int j = 0; j < N_EXTENSIONS; ++j) {
				std::filesystem::path img_path = std::filesystem::path(
					file_stem_str + '_' + MAP_TYPE_STRS[i] + EXTENSIONS[j]
				);
				if (std::filesystem::exists(img_path)) {
					_texture_infos[i] = res::load_texture(img_path);
					image_found = true;
					_map_loaded[i] = true;
					break;
				}
			}

			// if no possible image is found for the map type,
			// then a solid color texture is used.
			if (not image_found)
				_texture_infos[i] = res::create_solid_color(DEFAULT_COLORS[i]);
		}
	}
}

void Material::bind_to_GL() const {
	for (uint8_t i = 0; i < N_MAP_TYPES; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, _texture_infos[i]->texture_ID);
	}
}
} // namespace gu