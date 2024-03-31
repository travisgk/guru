#include "material.hpp"
#include "../image/load_texture.hpp"
#include "../image/color_texture.hpp"
#include "../image/texture_list.hpp"
#include <iostream>

namespace gu {
const std::string Material::MAP_TYPES[N_MAP_TYPES] = {
    "diffuse", "normal", "specular", "roughness", "displacement", "emissive"
};
const Color Material::DEFAULT_COLORS[N_MAP_TYPES] = {
    Color(0.5f, 0.5f, 0.5f), 
    Color(0.5f, 0.5f, 1.0f), 
    Color(0.0f, 0.0f, 0.0f), 
    Color(0.0f, 0.0f, 0.0f),
    Color(0.5f, 0.5f, 0.5f),
    Color(0.0f, 0.0f, 0.0f)
};

Material::~Material() {
    for (int i = 0; i < Material::N_MAP_TYPES; ++i)
        texture::TextureList::delete_texture(_texture_IDs[i]);
}

void Material::load_textures(const std::filesystem::path *paths) {
    std::string file_stem_str = paths[0].stem().string();
    std::string file_extension_str = paths[0].extension().string();
    _texture_IDs[0] = texture::load(paths[0], &_diffuse_path);
    for (int i = 1; i < N_MAP_TYPES; ++i) {
        if (not paths[i].empty() and std::filesystem::exists(paths[i])) {
            _texture_IDs[i] = texture::load(paths[i]);
        } else {
            static const std::string EXTENSIONS[] = {
                ".png", ".jpeg", ".jpg", ".bmp", ".gif"
            };
            static const int N_EXTENSIONS = (
                sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0])
            );

            bool image_found = false;
            for (int j = 0; j < N_EXTENSIONS; ++j) {
                std::filesystem::path img_path = std::filesystem::path(
                    file_stem_str + '_' + MAP_TYPES[i] + EXTENSIONS[j]
                );
                if (std::filesystem::exists(img_path)) {
                    _texture_IDs[i] = texture::load(img_path);
                    image_found = true;
                    _map_loaded[i] = true;
                    break;
                }
            }
            if (not image_found) {
                _texture_IDs[i] = texture::create_solid_color(
                    DEFAULT_COLORS[i]
                );
                _map_loaded[i] = false;
            }
        }
    }
}
} // namespace gu