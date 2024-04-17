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
#include "../resources/color.hpp"

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
	bool _is_used = true;
	uint8_t _n_samples = 4;

public:
	// dtor. deletes resources.
	~Screenbuffer();
	inline const GLsizei &get_width() const { return _width; }
	inline const GLsizei &get_height() const { return _height; }
	inline bool is_used() const { return _is_used; }

	// returns the image buffer ID.
	inline const GLuint &get_image_ID() const { return _image_buffer_ID; }

	// returns the intermediate buffer ID.
	inline const GLuint &get_inter_ID() const { 
		return _intermediate_buffer_ID; 
	}

	// returns the screen texture ID.
	inline const GLuint &get_screen_ID() const { 
		return _screen_texture_ID; 
	}

	inline void set_n_samples(uint8_t n_samples) {
		_is_used = n_samples > 0;
		_n_samples = std::clamp(
			n_samples, 
			static_cast<uint8_t>(1), 
			static_cast<uint8_t>(32)
		);
	}

	// allocates the textures and buffers of the Screenbuffer object.
	// this will destroy any pre-existing resources.
	bool create(const int &width, const int &height);

	// clears and binds the Screenbuffer 
	// so that its image buffer is being drawn to.
	void bind_and_clear(
		const gu::Color &clear_color = gu::Color(0.0, 0.0, 0.0)
	);

private:
	void _delete_resources();
};
} // namespace gu