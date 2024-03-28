/**
 * color_texture.hpp
 * ---
 * this file defines functions used 
 * to dynamically create OpenGL textures from given Color parameters.
 * 
 */

#pragma once
#include <glad/gl.h>
#include "color.hpp"

namespace gu {
namespace texture {
// returns the OpenGL ID of a created 2D texture that's one solid <color>.
GLuint create_solid_color(const gu::Color& color);

// returns the OpenGL ID of a created 2D texture 
// that's <center> color in the center and <edge> color along the boarders.
GLuint create_radial_gradient(
	const gu::Color& center, 
	const gu::Color& edge, 
	const int& resolution=256
);

// returns the OpenGL ID of a created 2D texture
// that has a checkerboard pattern of <width> pixels by <height> pixels.
GLuint create_checkerboard(
	const gu::Color& light,
	const gu::Color& dark,
	const int& width=16,
	const int& height=16
);

// returns a dynamically-created array of unsigned bytes
// which depict a checkboard. this data should be deleted after being used.
unsigned char *create_checkerboard_data(
	const gu::Color& light,
	const gu::Color& dark,
	const int& width,
	const int& height
);
}
}