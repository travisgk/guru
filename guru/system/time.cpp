#include "time.hpp"
#include <iostream> // debug
#if defined(_WIN32) 
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif
#include "settings.hpp"

namespace gu {
double Delta::_delta = 0.0;
double Delta::_fps = 0.0;
double Delta::_last_time = 0.0;
double Delta::_current_time = 0.0;

static void sleep_ms(const double &ms) {
	#if defined(_WIN32) 
	Sleep(static_cast<uint32_t>(ms));
	#elif defined(__linux__)
	usleep(static_cast<uint32_t>(ms * 1000));
	#endif
}

void Delta::update() {
	static const double MIN_FACTOR = 0.75;
	static const double MAX_FACTOR = 1.5;
	static bool last_frame_was_sluggish = false;
	_last_time = _current_time;
	_current_time = glfwGetTime();
	_delta = _current_time - _last_time;
	_fps = 1.0 / _delta;
}
} // namespace gu