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
	gu::env::reset(window);

	gu::PointLight point_light;
	point_light.place(0.0, 0.0, 9.0);

	gu::ModelResource *log = new gu::ModelResource("res/log/log.obj");

	gu::Transformation transformation_a;
	transformation_a.set_x(-2.0);
	transformation_a.set_y(-2.0);
	transformation_a.set_z(9.0);
	transformation_a.set_scaling(3.55f);
	transformation_a.update();

	gu::Transformation transformation_b;
	transformation_b.set_x(2.0);
	transformation_b.set_y(-2.0);
	transformation_b.set_z(9.0);
	transformation_b.set_scaling(3.55f);
	transformation_b.update();
	gu::LightShader light_shader;
	light_shader.build_from_files(
		"guru/shader/default_glsl/light_shader.v_shader", 
		"guru/shader/default_glsl/light_shader.f_shader"
	);

	GLuint texture_ID1 = gu::res::load_cube_map(
		"res/skybox/fishpond", ".jpg"
	);

	while (not window.should_close()) {
		gu::env::update_delta_and_poll_events();
		gu::env::clear_window_and_screenbuffer();

		transformation_a.rotate_y(0.2f);
		transformation_a.update();

		transformation_b.rotate_y(0.3f);
		transformation_b.update();

		light_shader.use();
		light_shader.set_view_pos(gu::env::get_camera().position());
		light_shader.set_point_light_pos(0, point_light.position());
		light_shader.set_point_light_diffuse(0, point_light.diffuse().rgb());
		light_shader.set_point_light_specular(0, point_light.specular().rgb());
		light_shader.set_point_light_constant(0, point_light.constant().value());
		light_shader.set_point_light_linear(0, point_light.linear().value());
		light_shader.set_point_light_quadratic(0, point_light.quadratic().value());



		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			
			gu::Camera &cam = gu::env::get_camera(i);
			cam.add_yaw(0.0f);
			cam.update();
			
			glm::mat4 PVM;
			PVM = cam.get_projview() * transformation_a.get_model_matrix();
			light_shader.set_PVM_mat(PVM);
			light_shader.set_model_mat(transformation_a.get_model_matrix());
			log->draw_opaque_meshes();
			log->draw_transparent_meshes();

			PVM = cam.get_projview() * transformation_b.get_model_matrix();
			light_shader.set_PVM_mat(PVM);
			light_shader.set_model_mat(transformation_b.get_model_matrix());
			log->draw_opaque_meshes();
			log->draw_transparent_meshes();

			gu::env::draw_skybox(cam.get_projview(), texture_ID1);
		}
		gu::env::display_frame();
	}
	
	delete log;
	gu::terminate();
	return 0;
}
