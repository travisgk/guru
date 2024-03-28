/**
 * load_texture.hpp
 * ---
 * this file defines functions used to load images into OpenGL as textures.
 * 
 */

#pragma once
#include <filesystem>
#include <glad/gl.h>
#include "color.hpp"

namespace gu {
namespace texture {
// returns the OpenGL ID of the loaded image.
// <smooth_on_mag> being true will upscale the texture 
// without sharpening the pixels.
GLuint load(const std::filesystem::path& path, const bool smooth_on_mag=true);

// returns the OpenGL ID of the loaded cube map.
// <smooth_on_mag> being true will upscale the texture 
// without sharpening the pixels.
// the <image_extension> should include the "." at the beginning.
// the cube map file names are expected to be:
// "posx", "negx", "posy", "negy", "posz", and "negz".
GLuint load_cube_map(
	const std::filesystem::path& directory, 
	const std::string& image_extension,
	const bool smooth_on_mag=true
);
}
}