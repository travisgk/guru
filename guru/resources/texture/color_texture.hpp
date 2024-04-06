/**
 * color_texture.hpp
 * ---
 * this file defines functions used 
 * to dynamically create OpenGL textures from given Color parameters.
 * 
 */

#pragma once
#include <filesystem>
#include <memory>
#include "../color.hpp"
#include "texture_info.hpp"

namespace gu {
namespace res {
// returns a shared pointer to a TextureInfo object that contains
// the OpenGL ID of a created 2D texture that's one solid <color>.
// if a solid color texture with the same parameters has already been created,
// then that pre-existing TextureInfo object will be returned.
std::shared_ptr<TextureInfo> create_solid_color(const gu::Color &color);

// returns a shared pointer to a TextureInfo object that contains
// the OpenGL ID of a created 2D texture that has 
// the <center> color in the center and <edge> color along the boarders.
// if a radial texture with the same parameters has already been created,
// then the TextureInfo pointer
// of that already existing texture will be returned.
std::shared_ptr<TextureInfo> create_radial_gradient(
	const gu::Color &center, const gu::Color &edge, const int &resolution=256
);

// returns a shared pointer to a TextureInfo object that contains
// the OpenGL ID of a created 2D texture that has 
// a checkerboard pattern of <width> pixels by <height> pixels.
// if a checkerboard texture with the same parameters has already been created,
// then the TextureInfo pointer
// of that already existing texture will be returned.
std::shared_ptr<TextureInfo> create_checkerboard(
	const gu::Color &light,
	const gu::Color &dark,
	const int &W=16,
	const int &H=16
);

// returns a dynamically-created array of unsigned bytes
// which depict a checkboard. this data should be deleted after being used.
unsigned char *create_checkerboard_data(
	const gu::Color &light,
	const gu::Color &dark,
	const int &width,
	const int &height
);
}
}