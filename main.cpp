#define GLFW_INCLUDE_NONE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <algorithm>
#include <iostream>
#include <assimp/Importer.hpp>
#include "guru/environment/environment.hpp"
#include "guru/resources/model/model_resource.hpp"
#include "guru/resources/material/material_list.hpp"
#include "guru/resources/texture/load_texture.hpp"
#include "guru/shader/model_shader.hpp"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main() {
	gu::init_GLFW();
	gu::Window window(WIDTH, HEIGHT, "Guru");
	gu::init_glad();
	gu::env::reset(window);

	gu::ModelResource *duck = new gu::ModelResource("res/duck/duck.obj");
	
	size_t duck_material_index = duck->get_material_index_by_path(
		"duck_material_a.png"
	);
	std::vector<gu::Material::Override> material_overrides_b;
	auto duck_material_b = gu::material_list.create_and_load(
		"res/duck/duck_material_b.png"
	);
	material_overrides_b.push_back(
		gu::Material::Override(duck_material_index, duck_material_b)
	);

	std::vector<gu::Material::Override> material_overrides_c;
	auto duck_material_c = gu::material_list.create_and_load(
		"res/duck/duck_material_c.png"
	);
	material_overrides_c.push_back(
		gu::Material::Override(duck_material_index, duck_material_c)
	);

	gu::Transformation transformation_a;
	transformation_a.set_x(-2.0);
	transformation_a.set_y(-1.5);
	transformation_a.set_z(9.0);
	transformation_a.set_scaling(0.15f);
	transformation_a.update();

	gu::Transformation transformation_b;
	transformation_b.set_x(2.0);
	transformation_b.set_y(-1.5);
	transformation_b.set_z(9.0);
	transformation_b.set_scaling(0.15f);
	transformation_b.update();

	gu::ModelShader model_shader;
	model_shader.build_from_files(
		"guru/shader/default_glsl/model_shader.v_shader", 
		"guru/shader/default_glsl/model_shader.f_shader"
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

		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			model_shader.use();
			gu::Camera &cam = gu::env::get_camera(i);
			cam.add_yaw(-0.01f);
			cam.update();
			
			glm::mat4 PVM = (
				cam.get_projview() * transformation_a.get_model_matrix()
			);
			model_shader.set_PVM_mat(PVM);
			duck->draw_opaque_meshes(material_overrides_b);
			duck->draw_transparent_meshes(material_overrides_b);

			PVM = (
				cam.get_projview() * transformation_b.get_model_matrix()
			);
			model_shader.set_PVM_mat(PVM);
			duck->draw_opaque_meshes(material_overrides_c);
			duck->draw_transparent_meshes(material_overrides_c);

			gu::env::draw_skybox(cam.get_projview(), texture_ID1);
		}
		gu::env::display_frame();
	}
	
	delete duck;
	gu::terminate();
	return 0;
}
