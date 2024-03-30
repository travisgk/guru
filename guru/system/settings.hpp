/**
 * settings.hpp
 * ---
 * this file defines the Settings struct, which holds all of Guru's settings.
 * 
 * ---
 * compiler flags for Guru are:
 * - #define GURU_DISABLE_TANGENTS_AND_BITANGENTS
 *    disables the implementation of tangents and bitangents in Vertices.
 * 
 * - #define GURU_AUTO_UPDATE_MATH_OBJECTS 
 *    changing attributes of an object of the "mathematics" module
 *    will run <update()> automatically.
 * 
 */

#pragma once

// dependencies compiler flags.
#define GLFW_INCLUDE_NONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <stdint.h>

namespace gu {
struct Settings {
public:
	static const uint8_t OPENGL_VERSION_MAJOR = 4;
	static const uint8_t OPENGL_VERSION_MINOR = 6;
private:
	static bool _vsync;
	static uint16_t _fps_limit;
	static int64_t _fps_pause_microseconds;
	
	// instances of this struct cannot be created.
	inline Settings() {}
public:
	inline static bool using_vsync() { return _vsync; }
	inline static uint16_t get_fps_limit() { return _fps_limit; }
	inline static const int64_t &get_fps_pause() {
		return _fps_pause_microseconds; 
	}
	inline static void set_vsync(bool activated) { _vsync = activated; }
	static void set_fps_limit(uint16_t limit);
};
}