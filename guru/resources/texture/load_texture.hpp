/**
 * load_texture.hpp
 * ---
 * this file defines functions used to load images into OpenGL as textures.
 * loaded textures will be maintained as shared pointers by the global
 * TextureList in order to prevent duplicate textures from being loaded.
 *
 */

#pragma once
#include <filesystem>
#include <memory>
#include "texture_info.hpp"
#include "../color.hpp"

namespace gu {
namespace res {
// returns the image format, based on the number of color channels.
GLenum determine_format(int n_color_channels);

// returns a shared pointer to a TextureInfo object that contains
// the OpenGL ID of the loaded image found at <path>.
// <is_transparent> is a boolean which when used will
// be set to 'true' if the program detects transparency.
// <smooth_on_mag> being true will upscale the texture
// without sharpening the pixels.
std::shared_ptr<TextureInfo> load_texture(
	const std::filesystem::path &path,
	bool *is_transparent=nullptr,
	const bool smooth_on_mag=true
);

// returns the OpenGL ID of the loaded cube map.
// <smooth_on_mag> being true will upscale the texture
// without sharpening the pixels.
// the <image_extension> should include the "." at the beginning.
// the cube map file names are expected to be:
// "posx", "negx", "posy", "negy", "posz", and "negz".
GLuint load_cube_map(
	const std::filesystem::path &directory,
	const std::string &image_extension,
	const bool smooth_on_mag=true
);
}
}