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

		for (int i = 0; i < gu::env::n_cameras(); i++) {
			gu::Camera &cam = gu::env::get_camera(i);
			cam.add_yaw(-0.3f);
			cam.update();
			gu::env::draw_skybox(cam.get_projview(), texture_ID1);
		}

		gu::env::postprocess_for_frame();
	}

	glfwTerminate();
	return 0;
}
