/**
 * settings.hpp
 * ---
 * this file defines the Settings struct, which holds all of Guru's settings.
 * macros can be defined here to compile Guru to particular specifications.
 * 
 * ---
 * macros for Guru are:
 * #define GURU_DISABLE_TANGENT_SPACE
 *    disables the implementation of tangents and bitangents in Vertices.
 *    Mesh, ModelResource
 * 
 * #define GURU_AUTO_UPDATE_MATH_OBJECTS 
 *    causes the changing attributes of an object
 *    of the "mathematics" module will run <update()> automatically.
 *
 * #define GURU_PRINT_RESOURCE_DEBUG_MESSAGES
 *    enables the ResourceList class 
 *    and all its children to print out messages for debugging.
 * 
 * #define GURU_USE_LEFT_HANDED_COORDINATES
 *    changes Guru to use Left-Handed coordinates.
 */

#pragma once
#define GURU_USE_LEFT_HANDED_COORDINATES
#include <stdint.h>
#include <string>
#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <iostream> // debug

namespace gu {
struct Settings {
public:
	static const uint8_t OPENGL_VERSION_MAJOR = 4;
	static const uint8_t OPENGL_VERSION_MINOR = 6;
	static const uint8_t N_GLFW_SAMPLES = 4;

private:
	static bool _vsync; // true if vertical synchronization is used
	static double _vsync_frame_duration; // duration (seconds) by monitor Hz
	static uint16_t _fps_limit; // used if <vsync> is false and is more than 0
	static double _fps_limit_duration; // duration (seconds) by fps limit
	
	// instances of this struct cannot be created.
	Settings() = delete;

public:
	inline static bool using_vsync() { return _vsync; }

	// returns the minimum duration that a frame should last under vsync.
	// this is used to prevent microstutters in the rendering.
	inline static const double &vsync_frame_duration() { 
		return _vsync_frame_duration; 
	}

	// returns the minimum duration that a frame should last
	// under the current set framerate limit.
	inline static uint16_t fps_limit() { return _fps_limit; }
	inline static const double &fps_limit_duration() {
		return _fps_limit_duration;
	}

	inline static void set_monitor_refresh_rate(const int &refresh_rate_hz) {
		_vsync_frame_duration = 1.0 / refresh_rate_hz;
		std::cout << refresh_rate_hz << " Hz" << std::endl; // debug
	}
	
	// sets the vsync setting. 
	// 0 will turn off all vsync.
	inline static void set_swap_interval(const int &swap_interval) { 
		glfwSwapInterval(swap_interval);
		_vsync = swap_interval != 0;
	}

	// imposes a framerate limit on the program.
	// 0 will remove any framerate limit and turn off vsync.
	static void set_fps_limit(uint16_t limit);
};
} // namespace gu