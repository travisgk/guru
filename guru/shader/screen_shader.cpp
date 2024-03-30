#include "screen_shader.hpp"

namespace gu {
void ScreenShader::_config_uniform_IDs() {
	glLinkProgram(_program_ID);
	GLint uni_screen_texture_ID = glGetUniformLocation(
		_program_ID, "_screen_texture_ID"
	);
	glLinkProgram(0);
	use();
	glUniform1i(uni_screen_texture_ID, 0);
	glUseProgram(0);
}
} // namespace gu