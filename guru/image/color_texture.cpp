#include "color_texture.hpp"
#include <string>
#include "texture_list.hpp"

namespace gu {
namespace texture {
GLuint create_solid_color(const gu::Color &color) {
	// returns the existing texture ID 
	// if the same configuration has been found.
	std::string path = "solid: " + color.to_str_255();
	GLuint texture_ID = TextureList::find_existing_texture_ID(path);
	if (texture_ID != NO_TEXTURE)
		return texture_ID;

	// creates a new texture in OpenGL memory and sets its parameters.
	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// uploads a 1x1 pixel image with the desired color.
	unsigned char fill[] = {
		color.unsigned_char_r(),
		color.unsigned_char_g(),
		color.unsigned_char_b(),
		color.unsigned_char_a()
	};
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		1, 
		1, 
		0,
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		fill
	);
	TextureList::add_entry(texture_ID, path);
	return texture_ID;
}

GLuint create_radial_gradient(
	const gu::Color &center, const gu::Color &edge, const int &resolution
) {
	// returns the existing texture ID 
	// if the same configuration has been found.
	std::string path = (
		  "radial (" + std::to_string(resolution) + "): " 
		+ center.to_str_255() + ", " + edge.to_str_255()
	);
	GLuint texture_ID = TextureList::find_existing_texture_ID(path);
	if (texture_ID != NO_TEXTURE)
		return texture_ID;

	// creates a new texture in OpenGL memory and sets its parameters.
	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// creates a pixel image.
	const int WIDTH = resolution;
	const int HEIGHT = resolution;
	const float MAX_DISTANCE = sqrt(
		(WIDTH / 2.0f) * (WIDTH / 2.0f) + 
		(HEIGHT / 2.0f) * (HEIGHT / 2.0f)
	);
	unsigned char *data = new unsigned char[WIDTH * HEIGHT * 4];

	// sets the RGBA values for each pixel.
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			// finds the distance from the center.
			float d_x = x - WIDTH / 2.0f;
			float d_y = y - HEIGHT / 2.0f;
			float distance = sqrt(d_x * d_x + d_y * d_y);

			// sets the Color based on the relative distance.
			float scale = distance / MAX_DISTANCE;
			Color inter = center.interpolate(edge, scale);
			const int PIXEL = (y * WIDTH + x) * 4;
			data[PIXEL + 0] = inter.unsigned_char_r();
			data[PIXEL + 1] = inter.unsigned_char_g();
			data[PIXEL + 2] = inter.unsigned_char_b();
			data[PIXEL + 3] = inter.unsigned_char_a();
		}
	}

	// uploads the created pixel image.
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		WIDTH, 
		HEIGHT, 
		0, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		data 
	);

	delete[] data;
	TextureList::add_entry(texture_ID, path);
	return texture_ID;
}

GLuint create_checkerboard(
	const gu::Color &light, 
	const gu::Color &dark, 
	const int &width, 
	const int &height 
) {
	// returns the existing texture ID 
	// if the same configuration has been found.
	std::string path = ("checker (" + std::to_string(width) + 
		", " + std::to_string(height) + "): " +
		light.to_str_255() + ", " + dark.to_str_255()
	);
	GLuint texture_ID = TextureList::find_existing_texture_ID(path);
	if (texture_ID != NO_TEXTURE)
		return texture_ID;

	// creates a new texture in OpenGL memory and sets its parameters.
	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// uploads the created pixel image.
	unsigned char *data = create_checkerboard_data(
		light, dark, width, height
	);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data
	);

	delete[] data;
	TextureList::add_entry(texture_ID, path);
	return texture_ID;
}

unsigned char *create_checkerboard_data(
	const gu::Color &light,
	const gu::Color &dark,
	const int &width,
	const int &height
) {
	unsigned char *data = new unsigned char[width * height * 4];
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const int PIXEL = (y * width + x) * 4;
			if (y * width + x % 2 == 0) {
				data[PIXEL + 0] = light.unsigned_char_r();
				data[PIXEL + 1] = light.unsigned_char_g();
				data[PIXEL + 2] = light.unsigned_char_b();
				data[PIXEL + 3] = light.unsigned_char_a();
			} else {
				data[PIXEL + 0] = dark.unsigned_char_r();
				data[PIXEL + 1] = dark.unsigned_char_g();
				data[PIXEL + 2] = dark.unsigned_char_b();
				data[PIXEL + 3] = dark.unsigned_char_a();
			}
		}
	}
	return data;
}
} // namespace texture
} // namespace gu