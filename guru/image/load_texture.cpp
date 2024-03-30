#include "load_texture.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture_list.hpp"
#include "../system/settings.hpp"
#include "color_texture.hpp"

static const gu::Color RAW_COLOR = gu::Color(1.0, 0.0, 0.0);
static const gu::Color ERROR_CHECKERBOARD_LIGHT = gu::Color(100, 0, 86);
static const gu::Color ERROR_CHECKERBOARD_DARK = gu::Color(0, 0, 0);

// returns the image format, based on the number of color channels.
static GLenum determine_format(int n_color_channels) {
	switch (n_color_channels) {
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	default:
		return GL_RED;
	}
}

namespace gu {
namespace texture {
GLuint load(const std::filesystem::path &path, const bool smooth_on_mag) {
	// returns the existing texture ID if the same <path> has been found.
	GLuint texture_ID = TextureList::find_existing_texture_ID(path);
	if (texture_ID != NO_TEXTURE)
		return texture_ID;

	// creates texture in OpenGL memory and sets its parameters.
	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR 
	);
	GLenum mag_option = smooth_on_mag ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_option);

	// loads image with stbi.
	int width, height, n_channels;
	unsigned char *data = stbi_load(
		path.string().c_str(), &width, &height, &n_channels, 0
	);

	if (data) {
		// transfers data to the texture.
		GLenum format = determine_format(n_channels);
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			format, 
			width, 
			height, 
			0, 
			format, 
			GL_UNSIGNED_BYTE, 
			data
		);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		TextureList::add_entry(texture_ID, path);
		return texture_ID;
	}

	std::cerr 
		<< "Image " << path.string() << " could not be loaded." 
		<< std::endl;
	stbi_image_free(data);
	glDeleteTextures(1, &texture_ID);
	GLuint error_ID = create_checkerboard(
		ERROR_CHECKERBOARD_LIGHT, ERROR_CHECKERBOARD_DARK
	);
	return error_ID;
}

GLuint load_cube_map(
	const std::filesystem::path &directory,
	const std::string &image_extension,
	const bool smooth_on_mag
) {
	// sets up the six image paths to be loaded from.
	const std::filesystem::path PATHS[6] = {
		directory / ("posx" + image_extension),
		directory / ("negx" + image_extension),
		directory / ("posy" + image_extension),
		directory / ("negy" + image_extension),
		directory / ("posz" + image_extension),
		directory / ("negz" + image_extension)
	};

	// creates texture in OpenGL memory and sets its parameters.
	GLuint texture_ID;
	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ID);
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
	);
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
	);
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE
	);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GLenum mag_option = smooth_on_mag ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_option
	);

	// loads the six images into the cube map texture.
	for (uint8_t i = 0; i < 6; ++i) {
		int width, height, n_channels;
		unsigned char *data = stbi_load(
			PATHS[i].string().c_str(), 
			&width, 
			&height, 
			&n_channels, 
			0
		);

		if (data) {
			// transfers data to the cube map.
			GLenum format = determine_format(n_channels);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				format,
				width,
				height,
				0,
				format,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		} else {
			std::cerr 
				<< "Image " << PATHS[i].string() 
				<< " could not be loaded." << std::endl;
			stbi_image_free(data);
			data = create_checkerboard_data(
				ERROR_CHECKERBOARD_LIGHT, 
				ERROR_CHECKERBOARD_DARK,
				16,
				16
			);
			GLenum map_i = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
			glTexImage2D(
				map_i, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
			delete[] data;
		}
	}
	return texture_ID;
}
} // namespace texture
} // namespace gu