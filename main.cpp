#include <iostream>
#include <assimp/Importer.hpp>
#include "guru/environment/environment.hpp"
#include "guru/system/settings.hpp"
#include "guru/system/window.hpp"
#include "guru/image/load_texture.hpp"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
	gu::init_GLFW();
	gu::Window window(WIDTH, HEIGHT, "Guru");
	gu::init_glad();
	gu::env::reset(window);

	// gu::settings.set_vsync(false);
	// gu::settings.set_fps_limit(10);
	GLuint texture_ID1 = gu::texture::load_cube_map(
		"res/skybox/meadow", ".jpg"
	);

	while (not window.should_close()) {
		gu::env::preprocess_for_frame();

		gu::env::get_camera()->move_forward(0.3f);

		gu::env::postprocess_for_frame();
	}

	glfwTerminate();
	return 0;
}
