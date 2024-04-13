/**
 * window.hpp
 * ---
 * this file defines the Window class, which enables easier management
 * of its internally-created GLFWwindow.
 *
 * it also includes functions for initializing the used libraries.
 * 
 */

#pragma once
#include <string>
#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <glm/vec2.hpp>

namespace gu {
class Window {
private:
	std::string _name;
	double _current_time = 0;
	double _last_time = 0;
	glm::ivec2 _prev_size;
	bool _window_destroyed = false;
	GLFWwindow *_window;

public:
	// ctor. runs _init(...) to create a GLFWwindow.
	Window(int width, int height, std::string name);
	
	// dtor. destroys the GLFWwindow properly.
	~Window();
	
	void destroy() {
		if (not _window_destroyed)
			return;
		glfwDestroyWindow(_window);
		_window_destroyed = true;
	}

	// deletes copy ctors to prevent access to the deleted GLFWwindow.
	Window(const Window&) = delete;
	Window &operator= (const Window&) = delete;
private:
	// creates a GLFWwindow and places it in the center of the monitor.
	void _init(int width, int height);

public:
	inline GLFWwindow *get_GLFWwindow() { return _window; }
	
	inline bool should_close() const { 
		return glfwWindowShouldClose(_window); 
	}

	inline void clear() { 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	}

	// makes the GLFWwindow fullscreen. 
	// <mode_num> provides the selection for a particular video mode.
	// -1 will use the default video mode.
	void make_fullscreen(int mode_num=-1);

	// makes the GLFWwindow windowed in its previous windowed size.
	// the window will also be placed in the center of the monitor.
	void make_windowed();

	// resets the GL viewport. 
	// this should be called before rendering the frame if a camera 
	// rendering to only a fraction of the window was used prior.
	void reset_viewport();

private:
	void _get_monitor_center(int &x, int &y) const;
};
}