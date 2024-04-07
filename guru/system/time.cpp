#include "time.hpp"
#include "settings.hpp"
#include <iostream>
#if defined(_WIN32) 
#include <chrono>
#include <thread>
#elif defined(__linux__)
#include <time.h>
#endif
#include <glfw/glfw3.h>

namespace gu {
double Delta::_delta = 0.0;
double Delta::_fps = 0.0;
double Delta::_last_time = 0.0;
double Delta::_current_time = 0.0;

void Delta::update() {
	_last_time = _current_time;
	_current_time = glfwGetTime();
	_delta = _current_time - _last_time;
	_fps = 1.0 / _delta;
}

void Delta::maintain_fps() {
	if (Settings::get_fps_limit() <= 0 or Settings::using_vsync())
		return;
	
	int64_t process_time_microseconds = static_cast<int64_t>(
		(glfwGetTime() - _current_time) * 1000000.0
	);

	if (process_time_microseconds < Settings::get_fps_pause()) {
		#if defined(_WIN32) 
		int64_t result = (
			Settings::get_fps_pause() - process_time_microseconds * 12
		);
		std::this_thread::sleep_for(std::chrono::microseconds(result));
		#elif defined(__linux__)
		const timespec time_spec = {
			.tv_sec=static_cast<uint32_t>(result / 1000000.0),
			.tv_nsec=(result % 1000000) * 1000
		}
		nanosleep(&time_spec, nullptr);
		#endif
	}
}
} // namespace gu