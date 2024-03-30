/**
 * environment.hpp
 * ---
 * this file defines the env struct, which helps simplify
 * the management of interfacing with OpenGL. it takes a Window object
 * and handles its events internally, updating the ScreenBuffer and Cameras.
 * 
 */

#pragma once
#include <memory>
#include <vector>
#include "../system/window.hpp"
#include "camera.hpp"
#include "../mathmatics/transformation.hpp"
#include "../image/color.hpp"
#include "../framebuffer/screenbuffer.hpp"
#include "../shader/screen_shader.hpp"
#include "../shader/skybox_shader.hpp"

namespace gu {
struct env {
private:
	static Window *_window;
	static GLuint _screen_display_VAO_ID;
	static GLuint _screen_display_VBO_ID;
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
	// dtor. properly deletes the screen display rectangle VAO and VBO.
	~env();
	inline static void set_clear_color(const gu::Color& color) {
		_clear_color = color;
	}

	// sets up the <_window> and <_cameras>,
	// sets up the <_screenbuffer>,
	// and builds the shaders.
	static void reset(Window& window);

private:
	static void _create_screen_display();
	static void _create_skybox();

public:
	// returns the index of a newly created Camera in memory.
	static size_t create_camera();

	// sets the paths to files used to build the <_screen_shader>.
	static void set_screen_shader_paths(
		const std::filesystem::path& v_shader_path,
		const std::filesystem::path& f_shader_path
	);

	// sets the paths to files used to build the <_skybox_shader>.
	static void set_skybox_shader_paths(
		const std::filesystem::path& v_shader_path,
		const std::filesystem::path& f_shader_path
	);

	// returns a reference to a specified Camera.
	// a negative <index> will return the most recently created Camera.
	static Camera& get_camera(int index = -1);

	inline static size_t n_cameras() { return _cameras.size();	}

	// draws a skybox, given a Camera's projview matrix
	// and an OpenGL cubemap ID.
	static void draw_skybox(
		const glm::mat4 &camera_projview_mat, GLuint cubemap_ID
	);

	// this function will maintain an FPS limit if enabled,
	// delta time will be updated, window events will be polled, 
	// and the Window and Screenbuffer will be set up for drawing.
	// this function should be called at the beginning on the main loop.
	static void preprocess_for_frame();

	// this function will update the matrices of any modified
	// mathematics object, swap buffers to display the Screenbuffer,
	// and handle vsync if enabled.
	// this function should be called at the end of the main loop.
	static void postprocess_for_frame();

private:
	// this function is called whenever the Window size is changed.
	static void _screenbuffer_size_callback(
		GLFWwindow *window, int width, int height
	);

	// updates the matrices of every object in the environment.
	static void _update_everything();

	// uses the <_screen_shader> to draw the contents of <_screenbuffer>.
	static void _display_frame();
};
} // namespace gu