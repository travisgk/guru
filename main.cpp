#include "example_animation.hpp"

int main() {
	gu::init_GLFW();
	gu::Window window(800, 600, "Guru");
	gu::init_glad();
	window.make_fullscreen();
	
	create_and_run_scene(window);
	
	window.destroy();
	gu::terminate();
	return 0;
}
