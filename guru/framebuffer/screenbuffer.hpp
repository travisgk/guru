/**
 * screenbuffer.hpp
 * ---
 * this file defines the Screenbuffer class, which is a buffer
 * that the scene is rendered to with quality enhancements (e.g. antialias).
 * 
 */

#pragma once
#include <algorithm>
#include <stdint.h>
#include <glad/gl.h>
#include "../image/color.hpp"

namespace gu {
struct Screenbuffer {
private:
	GLsizei _width = 640;
	GLsizei _height = 480;
	GLuint _image_buffer_ID = 0;
	GLuint _multisample_texture_ID = 0;
	GLuint _depth_buffer_ID = 0;
	GLuint _intermediate_buffer_ID = 0;
	GLuint _screen_texture_ID = 0;
	uint16_t _n_samples = 4;


public:
	// dtor. deletes resources.
	~Screenbuffer();
	inline const GLsizei &width() const { return _width; }
	inline const GLsizei &height() const { return _height; }

	// returns the image buffer ID.
	inline const GLuint &image_ID() const { return _image_buffer_ID; }

	// returns the intermediate buffer ID.
	inline const GLuint &inter_ID() const { 
		return _intermediate_buffer_ID; 
	}

	// returns the screen texture ID.
	inline const GLuint &screen_ID() const { 
		return _screen_texture_ID; 
	}

	inline void set_n_samples(uint16_t n_samples) {
		_n_samples = std::clamp(
			n_samples, 
			static_cast<uint16_t>(1), 
			static_cast<uint16_t>(8)
		);
	}

	// allocates the textures and buffers of the Screenbuffer object.
	// this will destroy any pre-existing resources.
	bool create(const int &width, const int &height);

	// binds the resources in order to be rendered to.
	void bind_and_clear(const gu::Color &clear_color=gu::Color(0.0, 0.0, 0.0));

private:
	void _delete_resources();
};
} // namespace gu