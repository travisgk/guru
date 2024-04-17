#include "load_texture.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture_list.hpp"
#include "color_texture.hpp"

static const gu::Color RAW_COLOR = gu::Color(1.0, 0.0, 0.0);
static const gu::Color ERROR_CHECKERBOARD_LIGHT = gu::Color(100, 0, 86);
static const gu::Color ERROR_CHECKERBOARD_DARK = gu::Color(0, 0, 0);
static auto &texture_list = gu::res::TextureList::texture_list;

namespace gu {
namespace res {
GLenum determine_format(int n_color_channels) {
	switch (n_color_channels) {
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	default:
		return GL_RED;
	}
}

std::shared_ptr<TextureInfo> load_texture(
	const std::filesystem::path &path,
	bool *is_transparent,
	const bool smooth_on_mag
) {
	// returns the existing texture ID if the same <path> has been found.
	const std::shared_ptr<TextureInfo> &ptr = texture_list.find_existing(path);
	if (ptr)
		return ptr;

	// creates texture in OpenGL memory and sets its parameters.
	GLuint texture_ID;
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
	int w, h, n_channels;
	unsigned char *data = stbi_load(
		path.string().c_str(), &w, &h, &n_channels, 0
	);

	if (data) {
		// transfers data to the texture.
		GLenum format = determine_format(n_channels);
		glTexImage2D(
			GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		// if the function is specified to check for transparency,
		// then a grid of pixels will be sampled
		// proportionally across the image to check if the texture is transparent.
		if (n_channels == 4 and is_transparent) {
			for (float x = 0.25f; x < 1.0f; x+=0.25f) {
				for (float y = 0.25f; x < 1.0f; x += 0.25f) {
					int center_pixel_i = static_cast<int>(
						h * y * w + w * x
					) * n_channels;
					if (data[center_pixel_i + 3] < 255) {
						*is_transparent = true;
						break;
					}
				}
				if (*is_transparent)
					break;
			}
		}
		stbi_image_free(data);
		TextureInfo new_info = TextureInfo(path, texture_ID);
		texture_list.create_entry(new_info);
		return texture_list.get_last_created();
	}

	std::cerr
		<< "Image " << path.string() << " could not be loaded."
		<< std::endl;
	stbi_image_free(data);
	glDeleteTextures(1, &texture_ID);

	const std::shared_ptr<TextureInfo> &error_info = create_checkerboard(
		ERROR_CHECKERBOARD_LIGHT,
		ERROR_CHECKERBOARD_DARK
	);
	return error_info;
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
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GLenum mag_option = smooth_on_mag ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_option);

	// loads the six images into the cube map texture.
	for (uint8_t i = 0; i < 6; ++i) {
		int w, h, n_channels;
		unsigned char *data = stbi_load(
			PATHS[i].string().c_str(), &w, &h, &n_channels, 0
		);

		GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		if (data) {
			// transfers data to the cube map.
			GLenum format = determine_format(n_channels);
			glTexImage2D(
				face, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		} else {
			std::cerr 
				<< "Image " << PATHS[i].string() 
				<< " could not be loaded." << std::endl;
			stbi_image_free(data);
			data = create_checkerboard_data(
				ERROR_CHECKERBOARD_LIGHT, ERROR_CHECKERBOARD_DARK, 16, 16
			);
			glTexImage2D(
				face, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
			delete[] data;
		}
	}
	return texture_ID;
}
} // namespace res
} // namespace gu