#include "screen_shader.hpp"
#include <iostream>

namespace gu {
void ScreenShader::_config_uniform_IDs() {
	glLinkProgram(_program_ID);
	_uni_screen_texture_ID = glGetUniformLocation(
		_program_ID, "_screen_texture_ID"
	);
	glLinkProgram(0);
	use();
	glUniform1i( _uni_screen_texture_ID, 0 );
}
} // namespace gu