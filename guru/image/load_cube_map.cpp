/**
* this file contains the implentation of <gu::texture::load_cube_map(...)>
* function defined in guru/image/texture.hpp.
*/

#include "texture.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gu {
namespace texture {
GLuint load_cube_map(
	const std::filesystem::path& directory, 
	const std::string& image_extension,
	const bool smooth_on_mag
) {
	// sets up the six image paths to be loaded from.
	const std::filesystem::path PATHS[6] = {
		directory / ( "right" + image_extension ),
		directory / ( "left" + image_extension ),
		directory / ( "top" + image_extension ),
		directory / ( "bottom" + image_extension ),
		directory / ( "back" + image_extension ),
		directory / ( "front" + image_extension )
	};

	// creates texture in OpenGL memory and sets its parameters.
	GLuint texture_ID;
	glGenTextures( 1, &texture_ID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, texture_ID );
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
	);
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
	);
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE
	);
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	GLenum mag_option = smooth_on_mag ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_option );

	// loads the six images into the cube map texture.
	for ( uint8_t i=0; i < 6; i++ ) {
		// loads image with stbi.
		int width, height, n_channels;
		unsigned char *data = stbi_load(
			PATHS[i].string().c_str(), &width, &height, &n_channels, 0
		);
		if ( data ) {
			// the data was successfully loaded. transfers data to the cube map.
			GLenum format = _determine_format( n_channels );
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
			stbi_image_free( data );
		} else {
			// the data failed to be loaded.
			std::cerr << "Image " << PATHS[i].string() << " could not be loaded."
								  << std::endl;
			stbi_image_free( data );
		}
	}
	return texture_ID;
}
}
}