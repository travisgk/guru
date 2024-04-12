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
	static const double MIN_OFFSET = 0.005;
	static const double MAX_OFFSET = 0.005;
	_last_time = _current_time;
	_current_time = glfwGetTime();
	_delta = _current_time - _last_time;

	if (Settings::using_vsync()) {
		if (Settings::fps_limit() == 0) {
			if (_delta > Settings::vsync_frame_duration() + MAX_OFFSET) {
				_delta = Settings::vsync_frame_duration();
			} else {
				// vsync without fps limit.
				// prevents microstutter in rendering.
				while (_delta < Settings::vsync_frame_duration() - MIN_OFFSET) {
					sleep_ms(1);
					_current_time = glfwGetTime();
					_delta = _current_time - _last_time;
				}
			}
		} else {
			const double &max_duration = (
				  Settings::vsync_frame_duration() 
				> Settings::fps_limit_duration() 
				? Settings::vsync_frame_duration() 
				: Settings::fps_limit_duration()
			);
			if (_delta > max_duration + MAX_OFFSET) {
				_delta = max_duration + +MAX_OFFSET;
			} else {
				// vsync with fps limit.
				while (_delta < max_duration - MIN_OFFSET) {
					sleep_ms(1);
					_current_time = glfwGetTime();
					_delta = _current_time - _last_time;
				} 
			}
		}
	} else if (Settings::fps_limit() > 0) {
		if(_delta > Settings::fps_limit_duration() + MAX_OFFSET) {
			_delta = Settings::fps_limit_duration();
		} else {
			// fps limit without vsync.
			while (_delta < Settings::fps_limit_duration() - MIN_OFFSET) {
				sleep_ms(Settings::fps_limit_duration() / 2);
				_current_time = glfwGetTime();
				_delta = _current_time - _last_time;
			}
		}
	}

	_fps = 1.0 / _delta;
}
} // namespace gu