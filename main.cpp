#define GLFW_INCLUDE_NONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GURU_AUTO_UPDATE_MATH_OBJECTS
#include <algorithm>
#include <iostream>
#include <assimp/Importer.hpp>
#include "guru/environment/environment.hpp"
#include "guru/environment/lights.hpp"
#include "guru/resources/model/model_resource.hpp"
#include "guru/resources/material/material_list.hpp"
#include "guru/resources/texture/load_texture.hpp"
#include "guru/shader/light_shader.hpp"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
	gu::Settings::set_vsync(true);
	gu::init_GLFW();
	gu::Window window(WIDTH, HEIGHT, "Guru");
	gu::init_glad();
	window.make_fullscreen();
	gu::env::reset(window);
	gu::env::activate_MSAA(4);

	gu::env::get_camera().place(0.0, 0.0, -70.0);

	gu::DirLight dir_light;
	dir_light.orient(glm::vec3(-1.0, -1.0, 0.0)); // x axis flipped..?
	dir_light.update();

	gu::PointLight point_light;
	point_light.place(0.0, 0.0, 0.0);

	gu::ModelResource *log = new gu::ModelResource("res/sphere/sphere.obj");

	static const size_t N_TRANSFORMATIONS = 6;
	std::vector<gu::Transformation> transformations;
	for (size_t i = 0; i < N_TRANSFORMATIONS; ++i) {
		transformations.push_back(gu::Transformation());
		transformations.back().set_scaling(1.6f);
		transformations.back().update();
	}

	gu::LightShader light_shader;
	light_shader.build_from_files(
		"guru/shader/default_glsl/light_shader.v_shader", 
		"guru/shader/default_glsl/light_shader.f_shader"
	);

	light_shader.use();
	light_shader.set_dir_light_dir(0, dir_light.direction_vec());
	light_shader.set_dir_light_diffuse(0, dir_light.diffuse().rgb());
	light_shader.set_dir_light_specular(0, dir_light.specular().rgb());
	
	light_shader.set_point_light_pos(0, point_light.position());
	light_shader.set_point_light_diffuse(0, point_light.diffuse().rgb());
	light_shader.set_point_light_specular(0, point_light.specular().rgb());
	light_shader.set_point_light_constant(0, point_light.constant().value());
	light_shader.set_point_light_linear(0, point_light.linear().value());
	light_shader.set_point_light_quadratic(0, point_light.quadratic().value());

	GLuint texture_ID1 = gu::res::load_cube_map(
		"res/skybox/fishpond", ".jpg"
	);

	while (not window.should_close()) {
		gu::env::update_delta_and_poll_events();
		gu::env::clear_window_and_screenbuffer();

		for (size_t i = 0; i < N_TRANSFORMATIONS; ++i) {
			transformations[i].rotate_y(0.2f + 0.05f * i);
			transformations[i].set_x(cos(glfwGetTime() * (0.5 - 0.01 * i)) * 3.5 * i);
			transformations[i].set_y(sin(glfwGetTime() * (0.5 - 0.01 * i)) * 3.5 * i);
			transformations[i].update();
		}

		light_shader.use();

		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			gu::Camera &cam = gu::env::get_camera(i);

			cam.move_right(5.0f);
			cam.update();
			cam.look_at(glm::vec3(0.0f));
			cam.update();
			light_shader.set_view_pos(cam.position());

			glm::mat4 PVM;
			for (size_t j = 0; j < N_TRANSFORMATIONS; ++j) {
				PVM = cam.get_projview() * transformations[j].get_model_matrix();
				light_shader.set_PVM_mat(PVM);
				light_shader.set_model_mat(transformations[j].get_model_matrix());
				log->draw_opaque_meshes();
				log->draw_transparent_meshes();
			}

			gu::env::draw_skybox(cam.get_skybox_mat(), texture_ID1);
		}
		gu::env::display_frame();
	}
	
	delete log;
	gu::terminate();
	return 0;
}
