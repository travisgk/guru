#define GLFW_INCLUDE_NONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GURU_AUTO_UPDATE_MATH_OBJECTS
#include <algorithm>
#include <iostream>
#include "guru/environment/environment.hpp"
#include "guru/environment/lights.hpp"
#include "guru/resources/model/model_resource.hpp"
#include "guru/resources/material/material_list.hpp"
#include "guru/shader/light_shader.hpp"
#include <iomanip> // debug

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
	gu::init_GLFW();
	gu::Window window(WIDTH, HEIGHT, "Guru");
	gu::init_glad();

	//window.make_fullscreen();
	gu::env::init(window);
	gu::env::activate_MSAA(32);
	
	// sets up lights and Camera.
	gu::Camera &cam = gu::env::camera();
	cam.place(0.0, 0.0, -230.0);
	cam.update();

	gu::DirLight dir_light;
	dir_light.orient(glm::vec3(-1.0, 0.0, 0.0));
	dir_light.diffuse().set(2.0, 2.0, 2.0);
	dir_light.update();

	gu::PointLight point_light;
	point_light.place(0.0, 0.0, 0.0);
	point_light.diffuse().set(0.0f, 0.0f, 0.0f);
	point_light.specular().set(0.0f, 0.0f, 0.0f);

	// loads models.
	gu::ModelResource *sphere = new gu::ModelResource("res/sphere/earth.obj");
	gu::ModelResource *arrow = new gu::ModelResource("res/arrow/smooth_arrow.obj");
	
	// sets up transformation for arrow following DirLight.
	gu::Transformation arrow_transformation = gu::Transformation();
	arrow_transformation.set_scaling(1.5f);
	arrow_transformation.update();

	// sets the material overrides for the axis arrows.
	auto x_ax_material = gu::material_list.create_and_load("res/arrow/arrow_orange.png");
	auto y_ax_material = gu::material_list.create_and_load("res/arrow/arrow_green.png");
	auto z_ax_material = gu::material_list.create_and_load("res/arrow/arrow_blue.png");
	size_t arrow_mat_index = arrow->get_material_index_by_path("arrow.png");
	const std::vector<gu::Material::Override> arrow_overrides[3] = {
		std::vector<gu::Material::Override>({gu::Material::Override(arrow_mat_index, x_ax_material)}),
		std::vector<gu::Material::Override>({gu::Material::Override(arrow_mat_index, y_ax_material)}),
		std::vector<gu::Material::Override>({gu::Material::Override(arrow_mat_index, z_ax_material)}),
	};

	// sets up the transformations for the axis arrows.
	gu::Transformation axes_transformations[3];
	for (uint8_t i = 0; i < 3; ++i) {
		glm::vec3 pos = glm::vec3(0.0f);
		pos[i] = 3.0f;
		axes_transformations[i].place(pos);
		
		glm::vec3 dir = glm::vec3(0.0f);
		dir[i] = 1.0f;
		axes_transformations[i].orient(dir);
		axes_transformations[i].set_scaling(2.0f);
		axes_transformations[i].update();
	}

	// sets up the transformation for the spheres.
	static const size_t N_TRANSFORMATIONS = 57;
	std::vector<gu::Transformation> transformations;
	for (size_t i = 0; i < N_TRANSFORMATIONS; ++i) {
		transformations.push_back(gu::Transformation());
		transformations.back().orient(0.0, 0.0, glm::radians(23.0));
		transformations.back().set_scaling(1.0f + 0.08f * i);
		transformations.back().update();
	}

	gu::SkyboxShader skybox_shader;
	
	// creates LightShader and sets constant light values.
	gu::LightShader light_shader;
	light_shader.build_from_files(
		"guru/shader/default_glsl/light_shader.v_shader", 
		"guru/shader/default_glsl/light_shader.f_shader"
	);
	
	light_shader.use();
	light_shader.set_ambient_color(glm::vec3(0.18, 0.18, 0.2));
	light_shader.set_dir_light_diffuse(0, dir_light.diffuse().rgb());
	light_shader.set_dir_light_specular(0, dir_light.specular().rgb());
	
	light_shader.set_point_light_pos(0, point_light.position());
	light_shader.set_point_light_diffuse(0, point_light.diffuse().rgb());
	light_shader.set_point_light_specular(0, point_light.specular().rgb());
	light_shader.set_point_light_constant(0, point_light.constant().value());
	light_shader.set_point_light_linear(0, point_light.linear().value());
	light_shader.set_point_light_quadratic(0, point_light.quadratic().value());

	// orients arrow to point in the direction of the DirLight.
	arrow_transformation.orient(dir_light.get_quat());
	arrow_transformation.update();

	// loads the skybox cubemap.
	GLuint cubemap_ID = gu::res::load_cube_map(
		"res/skybox/fishpond", ".jpg"
	);

	double start_time = glfwGetTime();
	uint64_t frame_num = 0;

	bool skip_next_frame = false;
	while (not window.should_close()) {
		double frame_start_time = glfwGetTime();
		//if (skip_next_frame) {
		//	skip_next_frame = false;
			//gu::env::display_frame();
			//double frame_time = glfwGetTime() - frame_start_time;
			//std::cout << "redisplay frame.\tframe time: " << frame_time << std::endl;
		//	continue;
		//}
			
		gu::env::poll_events_and_update_delta();
		
		// places every sphere at some position to create a spiral.
		double glfw_time = frame_start_time * 0.1;
		for (size_t i = 0; i < N_TRANSFORMATIONS; ++i) {
			static const double PROGRESS = 20.0;
			static const double SPIRAL = 0.5;
			static const int START_OFFSET = 18;
			transformations[i].set_x(cos(glfw_time + PROGRESS * (1.0 - 0.01 * (i + 1))) * SPIRAL * (1.0 + 0.01 * (i + START_OFFSET)) * (i + START_OFFSET));
			transformations[i].set_y(sin(glfw_time + PROGRESS * (1.0 - 0.01 * (i + 1))) * SPIRAL * (1.0 + 0.01 * (i + START_OFFSET)) * (i + START_OFFSET));
			transformations[i].add_yaw(-0.1f - 0.002f * (i + 1));
			transformations[i].update();
		}

		for (uint8_t i = 0; i < 3; ++i) {
			axes_transformations[i].move_forward(static_cast<float>(sin(frame_start_time) * 1.0f));
			axes_transformations[i].update();
		}

		// orbits Camera around (0, 0, 0).
		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			gu::Camera &cam = gu::env::camera(i);
			cam.move_right(15.0);
			cam.update();
			cam.look_at(glm::vec3(0.0f));
			cam.update();
		}

		// prepares for render.
		gu::env::clear_window_and_screenbuffer();
		light_shader.use();
		light_shader.set_dir_light_dir(0, dir_light.forward());
		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			gu::Camera &cam = gu::env::camera(i);
			light_shader.set_view_pos(cam.position());
			glm::mat4 PVM;
			
			// draws arrow that indicates the DirLight's direction.
			PVM = cam.get_projview() * arrow_transformation.get_model_matrix();
			light_shader.set_PVM_mat(PVM);
			light_shader.set_model_mat(arrow_transformation.get_model_matrix());
			arrow->draw_opaque_meshes();
			arrow->draw_transparent_meshes();

			// draws every sphere.
			for (size_t j = 0; j < N_TRANSFORMATIONS; ++j) {
				glm::mat4 model = transformations[j].get_model_matrix();
				PVM = cam.get_projview() * model;
				light_shader.set_PVM_mat(PVM);
				light_shader.set_model_mat(model);
				sphere->draw_opaque_meshes();
				sphere->draw_transparent_meshes();
			}

			// draws the axis arrows.
			for (uint8_t j = 0; j < 3; ++j) {
				PVM = cam.get_projview() * axes_transformations[j].get_model_matrix();
				light_shader.set_PVM_mat(PVM);
				light_shader.set_model_mat(axes_transformations[j].get_model_matrix());
				arrow->draw_opaque_meshes(arrow_overrides[j]);
				arrow->draw_transparent_meshes(arrow_overrides[j]);
			}

			gu::env::draw_skybox(cam.get_skybox_mat(), cubemap_ID);
		}
		gu::env::display_frame();
		
		// measures frame duration for irregularities.
		double frame_time = glfwGetTime() - frame_start_time;
		if (frame_time < gu::Settings::vsync_frame_duration() * 0.75) {
			std::cout << "blink frame. " << frame_num << "\tframe time: " << frame_time << std::endl;
			//skip_next_frame = true;
		}
		
		if (frame_time > gu::Settings::vsync_frame_duration() * 1.5) {
			std::cout << "slug frame. " << frame_num << "\tframe time: " << frame_time << std::endl;
		}
		++frame_num;
	}
	
	std::cout << glfwGetTime() - start_time << " seconds\tframe: " << frame_num << std::endl;
	delete sphere;
	delete arrow;
	window.destroy();
	gu::terminate();
	return 0;
}
