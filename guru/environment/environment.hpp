/**
 * environment.hpp
 * ---
 * this file defines the env class, which helps simplify
 * the management of interfacing with OpenGL. it takes a Window object
 * and handles its events internally, updating the Screenbuffer and Cameras.
 * 
 */

#pragma once
#include <memory>
#include <vector>
#include "../system/window.hpp"
#include "camera.hpp"
#include "../mathmatics/transformation.hpp"
#include "../resources/color.hpp"
#include "../resources/material/material_list.hpp"
#include "../resources/model/model_list.hpp"
#include "../resources/texture/texture_list.hpp"
#include "../system/screenbuffer.hpp"
#include "../shader/screen_shader.hpp"
#include "../shader/skybox_shader.hpp"

namespace gu {
static auto &model_res_list = gu::res::ModelResourceList::model_res_list;
static auto &material_list = gu::res::MaterialList::material_list;
static auto &texture_list = gu::res::TextureList::texture_list;

// returns true if the GLFW library was initialized successfully.
bool init_GLFW();

// returns true if the glad library was initialized successfully.
bool init_glad();

// deallocates loaded resources in the appropriate order.
void terminate();

class env {
private:
	static Window *_window;
	static GLuint _screen_display_VAO_ID; // screenbuffer display
	static GLuint _screen_display_VBO_ID; // screenbuffer display
	static GLuint _skybox_VAO_ID;
	static GLuint _skybox_VBO_ID;
	static std::vector<std::shared_ptr<Camera>> _cameras;
	static Screenbuffer _screenbuffer;
	static Color _clear_color;
	static std::filesystem::path _screen_shader_v_shader_path;
	static std::filesystem::path _screen_shader_f_shader_path;
	static std::filesystem::path _skybox_shader_v_shader_path;
	static std::filesystem::path _skybox_shader_f_shader_path;
	static ScreenShader _screen_shader;
	static SkyboxShader _skybox_shader;

	// instances of this struct cannot be created.
	inline env() {}
public:
	// dtor. properly deletes the screen display and skybox VAOs and VBOs.
	~env();

	inline static void set_clear_color(const gu::Color &color) {
		_clear_color = color;
	}

	// sets up the Guru environment with the given Window.
	static void reset(Window &window);

private:
	static void _create_screen_display();
	static void _create_skybox();

public:
	// returns the index of a newly created Camera in memory.
	static size_t create_camera();

	// sets the paths to files used to build the <_screen_shader>.
	static void set_screen_shader_paths(
		const std::filesystem::path &v_shader_path,
		const std::filesystem::path &f_shader_path
	);

	// sets the paths to files used to build the <_skybox_shader>.
	static void set_skybox_shader_paths(
		const std::filesystem::path &v_shader_path,
		const std::filesystem::path &f_shader_path
	);

	// returns a reference to a specified Camera.
	// a negative <index> will return the most recently created Camera.
	static Camera &get_camera(int index = -1);

	inline static size_t n_cameras() { return _cameras.size(); }

	// draws a skybox, given a Camera's projview matrix
	// and an OpenGL cubemap ID.
	static void draw_skybox(
		const glm::mat4 &camera_projview_mat, GLuint cubemap_ID
	);

	// updates the delta time and polls the Window for events.
	static void update_delta_and_poll_events();

	// sets up the Window and Screenbuffer for drawing.
	static void clear_window_and_screenbuffer();

	// displays the frame after everything has been drawn.
	static void display_frame();

private:
	// this function is called whenever the Window size is changed.
	// it will reallocate the Screenbuffer to match the new Window size.
	static void _screenbuffer_size_callback(
		GLFWwindow *window, int width, int height
	);

	// updates the matrices of every object in the environment.
	static void _update_everything();

	// binds the Screenbuffer's image buffer 
	// and blits it using the internal ScreenShader
	// to the Screenbuffer's intermediate buffer.
	static void _blit_frame_to_buffer();
};
} // namespace gu