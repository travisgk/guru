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
	gu::init_GLFW();
	gu::Window window(WIDTH, HEIGHT, "Guru");
	gu::init_glad();
	window.make_fullscreen();
	gu::Settings::set_fps_limit(12);

	gu::env::reset(window);
	gu::env::activate_MSAA(4);
	gu::env::get_camera().place(0.0, 0.0, -70.0);

	gu::DirLight dir_light;
	dir_light.orient(glm::vec3(1.0, -1.0, 0.0));
	dir_light.update();

	gu::PointLight point_light;
	point_light.place(0.0, 0.0, 0.0);
	point_light.diffuse().set(0.0f, 0.0f, 0.0f);
	point_light.specular().set(0.0f, 0.0f, 0.0f);

	gu::ModelResource *sphere = new gu::ModelResource("res/sphere/sphere_smooth.obj");

	gu::ModelResource *arrow = new gu::ModelResource("res/arrow/arrow_smooth.obj");
	gu::Transformation arrow_transformation = gu::Transformation();
	arrow_transformation.set_scaling(2.5f);
	arrow_transformation.update();

	static const size_t N_TRANSFORMATIONS = 100;
	std::vector<gu::Transformation> transformations;
	for (size_t i = 0; i < N_TRANSFORMATIONS; ++i) {
		transformations.push_back(gu::Transformation());
		transformations.back().set_scaling(0.3f);
		transformations.back().update();
	}

	gu::LightShader light_shader;
	light_shader.build_from_files(
		"guru/shader/default_glsl/light_shader.v_shader", 
		"guru/shader/default_glsl/light_shader.f_shader"
	);
	
	light_shader.use();
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

	double start_time = glfwGetTime();
	uint64_t frame_num = 0;

	while (not window.should_close()) {
		double frame_start_time = glfwGetTime();
		gu::env::poll_events_and_update_delta();
		
		dir_light.rotate_y(1.0f);
		dir_light.update();
		
		arrow_transformation.orient(dir_light.get_quat());
		arrow_transformation.update();

		double glfw_time = glfwGetTime();
		for (size_t i = 0; i < N_TRANSFORMATIONS; ++i) {
			transformations[i].rotate_y(0.2f + 0.05f * i);
			transformations[i].set_x(cos(glfw_time * (1.03 - 0.01 * (i + 1))) * 0.5 * (i + 10));
			transformations[i].set_y(sin(glfw_time * (1.03 - 0.01 * (i + 1))) * 0.5 * (i + 10));
			transformations[i].update();
		}
		
		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			gu::Camera &cam = gu::env::get_camera(i);
			cam.move_right(15.0f);
			cam.update();
			cam.look_at(glm::vec3(0.0f));
			cam.update();
		}

		gu::env::clear_window_and_screenbuffer();
		light_shader.use();
		light_shader.set_dir_light_dir(0, dir_light.direction_vec());
		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			gu::Camera &cam = gu::env::get_camera(i);
			light_shader.set_view_pos(cam.position());

			glm::mat4 PVM;
			PVM = cam.get_projview() * arrow_transformation.get_model_matrix();
			light_shader.set_PVM_mat(PVM);
			light_shader.set_model_mat(arrow_transformation.get_model_matrix());
			arrow->draw_opaque_meshes();
			arrow->draw_transparent_meshes();

			for (size_t j = 0; j < N_TRANSFORMATIONS; ++j) {
				PVM = cam.get_projview() * transformations[j].get_model_matrix();
				light_shader.set_PVM_mat(PVM);
				light_shader.set_model_mat(transformations[j].get_model_matrix());
				sphere->draw_opaque_meshes();
				sphere->draw_transparent_meshes();
			}
			gu::env::draw_skybox(cam.get_skybox_mat(), texture_ID1);
		}
		gu::env::display_frame();
		double frame_time = glfwGetTime() - frame_start_time;
		if (frame_time < 0.014 or frame_time > 0.02) {
			std::cout << "display " << std::to_string(frame_num) << "\tframe time: " << frame_time << std::endl;
		}
		++frame_num;
	}
	
	std::cout << glfwGetTime() - start_time << " seconds" << std::endl;

	delete sphere;
	gu::terminate();
	return 0;
}
