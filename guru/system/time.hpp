/**
 * time.hpp
 * ---
 * this file defines the DeltaTime struct, 
 * which manages the calculation of delta time.
 *
 */

#pragma once
#include "settings.hpp"

namespace gu {
struct Delta {
private:
	static double _delta;
	static double _fps;
	static double _last_time;
	static double _current_time;
	
	// instances of this struct cannot be created.
	Delta() = delete;

public:
	static inline double get() { return _delta; }
	static inline double fps() { return _fps; }

	// calculates the delta time and the fps.
	static void update();

	// makes the program manually pause to match <gu::Settings::get_fps_limit()>.
	// this only executes if <gu::Settings::use_vsync()> is false
	// and if <gu::Settings::get_fps_limit()> is above 0.
	static void maintain_fps();
};
}