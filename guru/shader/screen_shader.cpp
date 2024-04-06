#include "screen_shader.hpp"

namespace gu {
void ScreenShader::_config_uniform_IDs() {
	// finds IDs of uniform variables in the ScreenShader.
	glLinkProgram(_program_ID);
	GLint uni_screen_texture_1i_ID = glGetUniformLocation(
		_program_ID, "_screen_texture_ID"
	);
	glLinkProgram(0);

	// the screen texture will be expected to be bound to GL_TEXTURE0.
	use();
	glUniform1i(uni_screen_texture_1i_ID, 0);
	glUseProgram(0);
}
} // namespace gu