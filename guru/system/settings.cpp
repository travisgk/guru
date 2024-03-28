#include "settings.hpp"

namespace gu {
bool Settings::_vsync = true;
uint16_t Settings::_fps_limit = 0;
int64_t Settings::_fps_pause_microseconds = 0;
bool Settings::_maintains_texture_list = true;

void Settings::set_fps_limit(uint16_t limit) {
	_fps_limit = limit;
	if (_fps_limit > 0)
		_fps_pause_microseconds = 1000000 / _fps_limit;
}
} // namespace gu