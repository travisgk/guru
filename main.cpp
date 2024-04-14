#include "guru/environment/environment.hpp"

static void create_and_run_scene(gu::Window &window) {
	gu::env::init(window);
	gu::env::activate_MSAA(32);

	// sets up lights and Camera.
	gu::Camera &cam = gu::env::camera();
	cam.place(0.0, 0.0, -230.0);
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
	auto sphere = gu::model_res_list.create_and_load("res/sphere/earth.obj");
	auto arrow = gu::model_res_list.create_and_load("res/arrow/smooth_arrow.obj");

	// sets up transformation for arrow following DirLight.
	gu::Transformation arrow_transformation = gu::Transformation();
	arrow_transformation.set_scaling(1.5f);
	arrow_transformation.update();

	// creates the material overrides for the axis arrows.
	auto x_ax_material = gu::material_list.create_and_load("res/arrow/arrow_orange.png");
	auto y_ax_material = gu::material_list.create_and_load("res/arrow/arrow_green.png");
	auto z_ax_material = gu::material_list.create_and_load("res/arrow/arrow_blue.png");
	size_t arrow_mat_index = arrow->get_material_index_by_path("arrow.png");
	const std::vector<gu::Material::Override> arrow_overrides[3] = {
		std::vector<gu::Material::Override>(
			{gu::Material::Override(arrow_mat_index, x_ax_material)}
		),
		std::vector<gu::Material::Override>(
			{gu::Material::Override(arrow_mat_index, y_ax_material)}
		),
		std::vector<gu::Material::Override>(
			{gu::Material::Override(arrow_mat_index, z_ax_material)}
		)
	};

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

	// orients arrow to point in the direction of the DirLight.
	arrow_transformation.orient(dir_light.get_quat());
	arrow_transformation.update();

	// sets up the transformation for the spheres.
	static const size_t N_TRANSFORMATIONS = 57;
	std::vector<gu::Transformation> transformations;
	for (size_t i = 0; i < N_TRANSFORMATIONS; ++i) {
		transformations.push_back(gu::Transformation());
		transformations.back().orient(0.0, 0.0, glm::radians(23.0));
		transformations.back().set_scaling(1.0f + 0.08f * i);
		transformations.back().update();
	}

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

		// places every sphere at some position to create a spiral.
		double glfw_time = glfwGetTime();
		for (size_t i = 0; i < N_TRANSFORMATIONS; ++i) {
			static const double PROGRESS = 20.0;
			static const double SPIRAL = 0.5;
			static const int START_OFFSET = 18;
			transformations[i].set_x(
				cos(glfw_time * 0.1 + PROGRESS * (1.0 - 0.01 * (i + 1)))
				* SPIRAL
				* (1.0 + 0.01 * (i + START_OFFSET))
				* (i + START_OFFSET)
			);
			transformations[i].set_y(
				sin(glfw_time * 0.1 + PROGRESS * (1.0 - 0.01 * (i + 1)))
				* SPIRAL
				* (1.0 + 0.01 * (i + START_OFFSET))
				* (i + START_OFFSET)
			);
			transformations[i].add_yaw(-0.1f - 0.002f * (i + 1));
			transformations[i].update();
		}

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

			// draws arrow that indicates the DirLight's direction.
			PVM = cam.get_projview() * arrow_transformation.get_model_matrix();
			light_shader.set_PVM_mat(PVM);
			light_shader.set_model_mat(arrow_transformation.get_model_matrix());
			arrow->draw_meshes();

			// draws every sphere.
			for (size_t j = 0; j < N_TRANSFORMATIONS; ++j) {
				glm::mat4 model = transformations[j].get_model_matrix();
				PVM = cam.get_projview() * model;
				light_shader.set_PVM_mat(PVM);
				light_shader.set_model_mat(model);
				sphere->draw_meshes();
			}

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
