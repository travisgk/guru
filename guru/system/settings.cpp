#include "settings.hpp"

namespace gu {
bool Settings::_vsync = true;
double Settings::_vsync_frame_duration = 0.016;
uint16_t Settings::_fps_limit = 0;
double Settings::_fps_limit_duration = 0.016;

void Settings::set_fps_limit(uint16_t limit) {
	_fps_limit = limit;
	if (limit > 0)
		_fps_limit_duration = 1.0 / limit;
	else {
		_fps_limit_duration = 0.0;
		glfwSwapInterval(0);
		_vsync = false;
	}
}
} // namespace gu