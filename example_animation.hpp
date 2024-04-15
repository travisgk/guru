#include "guru/environment/environment.hpp"
#include <iostream>

static void create_and_run_scene(gu::Window &window) {
	gu::env::init(window);
	gu::env::activate_MSAA(32);

	// sets up lights and Camera.
	gu::Camera &cam = gu::env::camera();
	cam.place(0.0, 0.0, -30.0);
	cam.update();

	gu::DirLight dir_light;
	dir_light.orient(glm::vec3(-1.0, 0.0, 0.0));
	dir_light.diffuse().set(1.0f, 1.0f, 1.0f);
	dir_light.update();

	gu::PointLight point_light;
	point_light.place(0.0, 0.0, 0.0);
	point_light.diffuse().set(2.0f, 0.0f, 1.0f);
	point_light.linear().set(0.027f);
	point_light.quadratic().set(0.0028f);

	gu::SpotLight spot_light;

	// loads models.
	auto pants = gu::model_res_list.create_and_load("res/pants/pants.dae");
	gu::Animation animation = gu::Animation("res/pants/pants_animation.dae", *pants);
	gu::Animator animator;
	animator.set_animation(animation);

	auto arrow = gu::model_res_list.create_and_load("res/arrow/smooth_arrow.obj");

	// creates the material overrides for the axis arrows.
	auto x_ax_material = gu::material_list.create_and_load("res/arrow/arrow_orange.png");
	auto y_ax_material = gu::material_list.create_and_load("res/arrow/arrow_green.png");
	auto z_ax_material = gu::material_list.create_and_load("res/arrow/arrow_blue.png");
	int32_t arrow_mat_index = arrow->get_material_index_by_path("arrow.png");
	std::vector<gu::Material::Override> arrow_overrides[3];

	if (arrow_mat_index >= 0) {
		arrow_overrides[0] = { gu::Material::Override(arrow_mat_index, x_ax_material) };
		arrow_overrides[1] = { gu::Material::Override(arrow_mat_index, y_ax_material) };
		arrow_overrides[2] = { gu::Material::Override(arrow_mat_index, z_ax_material) };
	}

	// sets up the transformations for the axis arrows.
	gu::Transformation axes_tfs[3];
	for (uint8_t i = 0; i < 3; ++i) {
		glm::vec3 pos = glm::vec3(0.0f);
		pos[i] = 3.0f;
		axes_tfs[i].place(pos);

		glm::vec3 dir = glm::vec3(0.0f);
		dir[i] = 1.0f;
		axes_tfs[i].orient(dir);
		axes_tfs[i].set_scaling(2.0f);
		axes_tfs[i].update();
	}

	// sets up the transformation for the pants.
	gu::Transformation tf;
	tf.set_scaling(3.0f);
	tf.update();

	// creates LightShader and sets constant light values.
	gu::LightShader light_shader;
	light_shader.build_from_files(
		"guru/shader/default_glsl/light_shader.v_shader",
		"guru/shader/default_glsl/light_shader.f_shader"
	);
	light_shader.use();
	light_shader.set_ambient_color(glm::vec3(0.18, 0.18, 0.2));

	// loads the skybox cubemap.
	GLuint cubemap_ID = gu::res::load_cube_map(
		"res/skybox/fishpond", ".jpg"
	);

	while (not window.should_close()) {
		gu::env::poll_events_and_update_delta();
		animator.update_animation();
		
		const std::vector<glm::mat4> &bone_mats = animator.final_bone_matrices();
		for (const auto &bone_mat : bone_mats)
			std::cout << bone_mat[0][0] << '\t' << bone_mat[0][1] << '\t' << bone_mat[0][2] << '\t' << bone_mat[0][3] << std::endl;

		// places every sphere at some position to create a spiral.
		double glfw_time = glfwGetTime();

		for (uint8_t i = 0; i < 3; ++i) {
			axes_tfs[i].move_forward(static_cast<float>(sin(glfw_time) * 1.0f));
			axes_tfs[i].update();
		}

		// orbits Camera around (0, 0, 0).
		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			cam.move_right(15.0);
			cam.update();
			cam.look_at(glm::vec3(0.0f));
			cam.update();
		}

		// prepares for render.
		gu::env::clear_window_and_screenbuffer();
		light_shader.use();
		light_shader.update_GL_dir_light(0, dir_light);
		light_shader.update_GL_point_light(0, point_light);
		spot_light.place(gu::env::camera().position());
		spot_light.orient(gu::env::camera().get_quat());
		for (int i = 0; i < gu::env::n_cameras(); ++i) {
			gu::Camera &cam = gu::env::camera(i);
			light_shader.set_view_pos(cam.position());
			glm::mat4 PVM;

			// draws every sphere.
			glm::mat4 model = tf.get_model_matrix();
			PVM = cam.get_projview() * model;
			light_shader.set_PVM_mat(PVM);
			light_shader.set_model_mat(model);
			pants->draw_meshes();

			// draws the axis arrows.
			for (uint8_t j = 0; j < 3; ++j) {
				PVM = cam.get_projview() * axes_tfs[j].get_model_matrix();
				light_shader.set_PVM_mat(PVM);
				light_shader.set_model_mat(axes_tfs[j].get_model_matrix());
				arrow->draw_meshes(arrow_overrides[j]);
			}

			gu::env::draw_skybox(cam.get_skybox_mat(), cubemap_ID);
		}
		gu::env::display_frame();
	}
}