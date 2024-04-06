/**
 * screen_shader.hpp
 * ---
 * this file defines the ScreenShader class as a child of the Shader class,
 * which is built to display the Screenbuffer in the Window.
 * 
 */

#pragma once
#include "shader.hpp"

namespace gu {
class ScreenShader : public Shader {
protected:
	// sets the class's contained uniform IDs by searching for them in the code.
	virtual void _config_uniform_IDs() override;
};
} // namespace gu