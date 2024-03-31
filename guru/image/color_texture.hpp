/**
 * color_texture.hpp
 * ---
 * this file defines functions used 
 * to dynamically create OpenGL textures from given Color parameters.
 * 
 */

#pragma once
#include <filesystem>
#include <glad/gl.h>
#include "color.hpp"

namespace gu {
namespace texture {
// returns the OpenGL ID of a created 2D texture that's one solid <color>.
// if a solid color texture with the same parameters has already been created,
// then the ID of that already existing texture will be returned.
// <output_path> can be used to retrieve the image path
// decided upon by the program.
GLuint create_solid_color(
	const gu::Color &color, std::filesystem::path *output_path=nullptr
);

// returns the OpenGL ID of a created 2D texture 
// that's <center> color in the center and <edge> color along the boarders.
// if a radial texture with the same parameters has already been created,
// then the ID of that already existing texture will be returned.
// <output_path> can be used to retrieve the image path
// decided upon by the program.
GLuint create_radial_gradient(
	const gu::Color &center, 
	const gu::Color &edge, 
	std::filesystem::path *output_path=nullptr,
	const int &resolution=256
);

// returns the OpenGL ID of a created 2D texture
// that has a checkerboard pattern of <width> pixels by <height> pixels.
// if a checkerboard texture with the same parameters has already been created,
// then the ID of that already existing texture will be returned.
// <output_path> can be used to retrieve the image path
// decided upon by the program.
GLuint create_checkerboard(
	const gu::Color &light,
	const gu::Color &dark,
	std::filesystem::path *output_path=nullptr,
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