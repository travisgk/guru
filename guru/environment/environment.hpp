/**
 * environment.hpp
 * ---
 * this file defines the env class, which helps simplify
 * the management of interfacing with OpenGL. it takes a Window object
 * and handles its events internally, updating the Screenbuffer and Cameras.
 * 
 */

#pragma once
#include "camera.hpp"
#include "../mathmatics/transformation.hpp"
#include "../resources/animation/animator.hpp"
#include "../resources/material/material_list.hpp"
#include "../resources/model/model_list.hpp"
#include "../resources/texture/load_texture.hpp"
#include "../resources/texture/texture_list.hpp"
#include "../shader/light_shader.hpp"
#include "../shader/screen_shader.hpp"
#include "../shader/skybox_shader.hpp"
#include "../system/screenbuffer.hpp"
#include "../system/window.hpp"

namespace gu {
static auto &model_res_list = gu::res::ModelResourceList::model_res_list;
static auto &material_list = gu::res::MaterialList::material_list;
static auto &texture_list = gu::res::TextureList::texture_list;

// returns true if the GLFW library was initialized successfully.
bool init_GLFW();

// returns true if the glad library was initialized successfully.
bool init_glad();

// deallocates loaded resources in the appropriate order,
// properly deletes the screen display and skybox VAOs and VBOs,
// and finally terminates Guru's used libraries.
void terminate();

class env {
private:
	static Window *_window;
	static GLuint _screen_display_VAO_ID; // screenbuffer display
	static GLuint _screen_display_VBO_ID; // screenbuffer display
	static GLuint _skybox_VAO_ID;
	static GLuint _skybox_VBO_ID;
	static std::vector<Camera> _cameras;
	static Screenbuffer _screenbuffer;
	static Color _clear_color;
	static ScreenShader _default_screen_shader;
	static SkyboxShader _default_skybox_shader;
	friend void terminate();

	// instances of this class cannot be created.
	inline env() {}
public:
	// sets up the Guru environment with the given Window
	// so that the Screenbuffer updates with it correctly.
	// creates one Camera as well.
	static void init(Window &window);

	// resets the Guru environment to its default settings.
	static void reset();
private:
	static void _create_screen_display();
	static void _create_skybox();

public:
	// returns a reference to a specified Camera.
	// a negative <index> will return the most recently created Camera.
	static Camera &camera(int index = -1);

	// returns the number of Cameras currently used.
	inline static size_t n_cameras() { return _cameras.size(); }

	// returns the index of a newly created Camera in memory.
	static size_t create_camera();

	inline static void set_clear_color(const gu::Color &color) {
		_clear_color = color;
	}

	// turns on MSAA antialiasing, which will utilize the Screenbuffer.
	// giving 0 for <n_multisamples> will turn the Screenbuffer off.
	static void activate_MSAA(uint8_t n_multisamples);

	// draws a skybox with a default SkyboxShader,
	// the given Camera's skybox matrix, and an OpenGL cubemap ID.
	static void draw_skybox(
		const glm::mat4 &cam_skybox_mat, GLuint cubemap_ID
	);

	// draws a skybox with the given <skybox_shader>,
	// the Camera's skybox matrix, and an OpenGL cubemap ID.
	static void draw_skybox(
		const SkyboxShader &skybox_shader, 
		const glm::mat4 &cam_skybox_mat, 
		GLuint cubemap_ID
	);

	// polls the Window for events and updates the delta time.
	static void poll_events_and_update_delta();

	// sets up the Window and Screenbuffer for drawing.
	static void clear_window_and_screenbuffer();

	// displays the frame after everything has been drawn.
	// the frame is drawn using a default ScreenShader.
	static void display_frame();

	// displays the frame after everything has been drawn.
	// the frame is drawn using the <screen_shader>.
	static void display_frame(const ScreenShader &screen_shader);

private:
	// this function is called whenever the Window size is changed.
	// it will reallocate the Screenbuffer to match the new Window size.
	static void _screenbuffer_size_callback(
		GLFWwindow *window, int width, int height
	);

	// binds the Screenbuffer's image buffer 
	// and blits it using the internal ScreenShader
	// to the Screenbuffer's intermediate buffer.
	static void _blit_frame_to_buffer();
};
} // namespace gu