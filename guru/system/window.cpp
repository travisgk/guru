#include "window.hpp"
#include <iostream>
#include <stdint.h>
#include "settings.hpp"
#include "../resources/texture/stb_image.h"

namespace gu {
Window::Window(int width, int height, std::string name) : _name(name) { 
	_init(width, height); 
}

Window::~Window() { 
	glfwDestroyWindow(_window); 
}

void Window::_init(int width, int height) {
	_window = glfwCreateWindow(
		width, height, _name.c_str(), nullptr, nullptr 
	);

	if (_window == nullptr) {
		std::cerr << "GLFW failed to create a new window.\n";
		return;
	}
	_prev_size = glm::vec2(width, height);
	glfwMakeContextCurrent(_window);
	make_windowed();

	// attempts to load an image for the Window's icon.
	GLFWimage images[1] = {GLFWimage()};
	images[0].pixels = stbi_load(
		"res/icon.png", &images[0].width, &images[0].height, 0, 4
	);
	glfwSetWindowIcon(_window, 1, images);
	stbi_image_free(images[0].pixels);
}

void Window::make_fullscreen(int mode_num) {
	int width, height;
	glfwGetWindowSize(_window, &width, &height);
	_prev_size = glm::vec2(width, height);
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	
	if (mode_num >= 0) {
		int count;
		const GLFWvidmode *modes = glfwGetVideoModes(monitor, &count);
		if (not modes) {
			std::cerr << "GLFW failed to get video modes.\n";
			return;
		} else if (mode_num < count) {
			const GLFWvidmode &mode = modes[mode_num];
			glfwSetWindowMonitor(
				_window, 
				monitor, 
				0, 
				0, 
				mode.width,
				mode.height,
				mode.refreshRate
			);
			return;
		}
	}
	GLFWvidmode const *mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(
		_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate
	);
}

void Window::make_windowed() {
	int x, y;
	_get_monitor_center(x, y);
	glfwSetWindowMonitor(
		_window, nullptr, x, y, _prev_size.x, _prev_size.y, GLFW_DONT_CARE
	);
	glfwSetWindowSize(_window, _prev_size.x, _prev_size.y);
}

void Window::reset_viewport() {
	int width, height;
	glfwGetWindowSize(_window, &width, &height);
	_prev_size = glm::vec2(width, height);
	glViewport(0, 0, width, height);
}

void Window::_get_monitor_center(int &x, int &y) const {
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	if (monitor) {
		GLFWvidmode const *mode = glfwGetVideoMode(monitor);
		if (mode) {
			int monitor_x, monitor_y;
			glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);
			x = static_cast<int>(
				monitor_x + mode->width / 2.0 - _prev_size.x / 2.0
			);
			y = static_cast<int>(
				monitor_y + mode->height / 2.0 - _prev_size.y / 2.0
			);
			return;
		}
	}
	x = 0;
	y = 0;
}
} // namespace gu