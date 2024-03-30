#include "material.hpp"
#include "../image/load_texture.hpp"
#include "../image/color_texture.hpp"
#include "../image/texture_list.hpp"

namespace gu {
const std::string Material::MAP_TYPES[6] = { 
    "diffuse", "normal", "specular", "roughness", "displacement", "reflective" 
};
const Color Material::DEFAULT_COLORS[6] = {
    Color(0.5f, 0.5f, 0.5f), 
    Color(0.5f, 0.5f, 1.0f), 
    Color(0.0f, 0.0f, 0.0f), 
    Color(0.0f, 0.0f, 0.0f),
    Color(0.5f, 0.5f, 0.5f),
    Color(0.0f, 0.0f, 0.0f)
};

Material::~Material() {
    for (int i = 0; i < 6; ++i)
        texture::TextureList::delete_texture(_texture_IDs[i]);
}

void Material::load_textures(const std::filesystem::path &path) {
    std::string file_stem_str = path.stem().string();
    std::string file_extension_str = path.extension().string();
    _texture_IDs[0] = texture::load(path);
    for (int i = 1; i < 6; ++i) {
        std::filesystem::path img_path = std::filesystem::path(
            file_stem_str + '_' + MAP_TYPES[i] + file_extension_str
        );
        if (std::filesystem::exists(img_path))
            _texture_IDs[i] = texture::load(img_path);
        else
            _texture_IDs[i] = texture::create_solid_color(DEFAULT_COLORS[i]);
    }
}
} // namespace gu