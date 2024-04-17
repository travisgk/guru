#include "screenbuffer.hpp"
#include <iostream>

#if defined(_WIN32)
static const GLenum INTERNAL_FORMAT = GL_RGB;
#elif defined(__linux__)
static const GLenum INTERNAL_FORMAT = GL_RGB16F;
#endif
static const gu::Color RAW_COLOR = gu::Color(1.0, 0.0, 0.0); // clear color

namespace gu {
Screenbuffer::~Screenbuffer() {
	_delete_resources();
}

bool Screenbuffer::create(const int &width, const int &height) {
	if (width <= 0 or height <= 0)
		return false;
	_delete_resources();

	// creates the image buffer.
	glGenFramebuffers(1, &_image_buffer_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, _image_buffer_ID);

	// creates the multisample texture.
	_width = static_cast<GLsizei>(width);
	_height = static_cast<GLsizei>(height);
	glGenTextures(1, &_multisample_texture_ID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _multisample_texture_ID);
	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE,
		_n_samples,
		INTERNAL_FORMAT,
		_width,
		_height,
		GL_TRUE
	);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// binds the multisample texture to the image buffer.
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D_MULTISAMPLE,
		_multisample_texture_ID,
		0
	);

	// creates the depth buffer.
	glGenRenderbuffers(1, &_depth_buffer_ID);
	glBindRenderbuffer(GL_RENDERBUFFER, _depth_buffer_ID);
	glRenderbufferStorageMultisample(
		GL_RENDERBUFFER,
		_n_samples,
		GL_DEPTH24_STENCIL8,
		_width,
		_height
	);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// binds the depth buffer to the image buffer.
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER,
		_depth_buffer_ID
	);

	// checks if the build was successful.
	GLenum buffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (buffer_status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr
			<< "Screenbuffer: the Framebuffer of "
			<< "<_image_buffer_ID> failed to be created."
			<< std::endl;
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// creates the intermediate buffer.
	glGenFramebuffers(1, &_intermediate_buffer_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, _intermediate_buffer_ID);

	// creates the output screen texture.
	glGenTextures(1, &_screen_texture_ID);
	glBindTexture(GL_TEXTURE_2D, _screen_texture_ID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		INTERNAL_FORMAT,
		_width,
		_height,
		0,
		GL_RGB, // UNCERTAIN
		GL_UNSIGNED_BYTE,
		nullptr
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// binds the output screen texture to the intermediate buffer.
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		_screen_texture_ID,
		0
	);
	glEnable(GL_STENCIL_TEST);

	// checks if the build was successful.
	buffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (buffer_status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr
			<< "Screenbuffer: the Framebuffer of "
			<< "<_intermediate_buffer_ID> failed to be created."
			<< std::endl;
		return false;
	}
	return true;
}

void Screenbuffer::bind_and_clear(const gu::Color &clear_color) {
	// clears Window in preparation.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(
		RAW_COLOR.get_r(),
		RAW_COLOR.get_g(),
		RAW_COLOR.get_b(),
		RAW_COLOR.get_a()
	);
	glClear(
		  GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT
		| GL_STENCIL_BUFFER_BIT
	);

	// binds the Screenbuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, _image_buffer_ID);
	glClearColor(
		clear_color.get_r(),
		clear_color.get_g(),
		clear_color.get_b(),
		clear_color.get_a()
	);
	glClear(
		  GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT
		| GL_STENCIL_BUFFER_BIT
	);
	glEnable(GL_DEPTH_TEST);
}

void Screenbuffer::_delete_resources() {
	if (_screen_texture_ID != 0)
		glDeleteTextures(1, &_screen_texture_ID);

	if (_intermediate_buffer_ID != 0)
		glDeleteFramebuffers(1, &_intermediate_buffer_ID);

	if (_depth_buffer_ID != 0)
		glDeleteRenderbuffers(1, &_depth_buffer_ID);

	if (_multisample_texture_ID != 0)
		glDeleteTextures(1, &_multisample_texture_ID);

	if (_image_buffer_ID != 0)
		glDeleteFramebuffers(1, &_image_buffer_ID);

	_image_buffer_ID = 0;
	_multisample_texture_ID = 0;
	_depth_buffer_ID = 0;
	_intermediate_buffer_ID = 0;
	_screen_texture_ID = 0;
}
} // namespace gu