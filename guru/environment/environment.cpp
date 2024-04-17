#if defined(_WIN32)
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif
#include "environment.hpp"
#include "../system/settings.hpp"
#include <iostream>

static const std::filesystem::path DEF_SCREEN_SHADER_V_PATH = (
	"guru/shader/default_glsl/_screen_shader.v_shader"
);
static const std::filesystem::path DEF_SCREEN_SHADER_F_PATH = (
	"guru/shader/default_glsl/_screen_shader.f_shader"
);
static const std::filesystem::path DEF_SKYBOX_SHADER_V_PATH = (
	"guru/shader/default_glsl/_skybox_shader.v_shader"
);
static const std::filesystem::path DEF_SKYBOX_SHADER_F_PATH = (
	"guru/shader/default_glsl/_skybox_shader.f_shader"
);

namespace gu {
Window *env::_window;
GLuint env::_screen_display_VAO_ID = 0;
GLuint env::_screen_display_VBO_ID = 0;
GLuint env::_skybox_VAO_ID = 0;
GLuint env::_skybox_VBO_ID = 0;
std::vector<Camera> env::_cameras;
Screenbuffer env::_screenbuffer;
Color env::_clear_color = gu::Color(0.3f, 0.3f, 0.3f);
ScreenShader env::_default_screen_shader;
SkyboxShader env::_default_skybox_shader;

bool init_GLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cerr << "GLFW failed to initialize.\n";
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Settings::OPENGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Settings::OPENGL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// adds a bit more MSAA on top
	if (Settings::N_GLFW_SAMPLES > 0)
		glfwWindowHint(GLFW_SAMPLES, Settings::N_GLFW_SAMPLES);
	return true;
}

bool init_glad() {
	if (not gladLoadGL(GLADloadfunc(glfwGetProcAddress))) {
		std::cerr << "glad failed to initialize." << std::endl;
		return false;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glDepthFunc(GL_LESS);
	return true;
}

void terminate() {
	model_res_list.deallocate();
	material_list.deallocate();
	texture_list.deallocate();

	if (env::_screen_display_VBO_ID != 0)
		glDeleteBuffers(1, &env::_screen_display_VBO_ID);
	if (env::_screen_display_VAO_ID != 0)
		glDeleteVertexArrays(1, &env::_screen_display_VAO_ID);
	env::_screen_display_VBO_ID = 0;
	env::_screen_display_VAO_ID = 0;

	if (env::_skybox_VBO_ID != 0)
		glDeleteBuffers(1, &env::_skybox_VBO_ID);
	if (env::_skybox_VAO_ID != 0)
		glDeleteVertexArrays(1, &env::_skybox_VAO_ID);
	env::_skybox_VBO_ID = 0;
	env::_skybox_VAO_ID = 0;

	glfwTerminate();
}

void env::init(Window &window) {
	_window = &window;
	set_clear_color(gu::Color(0.3f, 0.3f, 0.3f));
	_create_screen_display();
	_create_skybox();
	create_camera();

	int width, height;
	glfwGetWindowSize(_window->get_GLFWwindow(), &width, &height);
	_screenbuffer_size_callback(_window->get_GLFWwindow(), width, height);
	glfwSetFramebufferSizeCallback(
		_window->get_GLFWwindow(), _screenbuffer_size_callback
	);

	_default_screen_shader.build_from_files(
		DEF_SCREEN_SHADER_V_PATH, DEF_SCREEN_SHADER_F_PATH
	);
	_default_skybox_shader.build_from_files(
		DEF_SKYBOX_SHADER_V_PATH, DEF_SKYBOX_SHADER_F_PATH
	);
}

// sets up the <_window> and <_cameras>,
// sets up the <_screenbuffer>,
// and builds the shaders.
void env::reset() {
	set_clear_color(gu::Color(0.3f, 0.3f, 0.3f));
	std::vector<Camera>().swap(_cameras);
	create_camera();
}

// creates the VAO and VBO for the rectangle used to show the <_screenbuffer>.
void env::_create_screen_display() {
	if (_screen_display_VAO_ID != 0 or _screen_display_VBO_ID != 0)
		return;
	
	static const float VERTICES[24] = {
		// position        texCoords
		-1.0f,  1.0f,     0.0f, 1.0f,
		-1.0f, -1.0f,     0.0f, 0.0f,
		 1.0f, -1.0f,     1.0f, 0.0f,
		-1.0f,  1.0f,     0.0f, 1.0f,
		 1.0f, -1.0f,     1.0f, 0.0f,
		 1.0f,  1.0f,     1.0f, 1.0f
	};
	
	// creates and binds the VAO and VBO.
	glGenVertexArrays(1, &_screen_display_VAO_ID);
	glGenBuffers(1, &_screen_display_VBO_ID);
	glBindVertexArray(_screen_display_VAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, _screen_display_VBO_ID);

	// transfers data to video card.
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), &VERTICES, GL_STATIC_DRAW);

	// specifies to the video card how the vertices should be interpreted.
	void* ptr = (void*)0;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), ptr);
	ptr = (void*)(2 * sizeof(float));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), ptr);

	// disconnects the VAO and VBO from the system.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// creates the VAO and VBO for the cube used to render the skybox.
void env::_create_skybox() {
	if (_skybox_VAO_ID != 0 or _skybox_VBO_ID != 0)
		return;

	static const float VERTICES[108] = {
		-1.0f,  1.0f, -1.0f,	-1.0f, -1.0f, -1.0f,	 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,	 1.0f,  1.0f, -1.0f,	-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,	-1.0f, -1.0f, -1.0f,	-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,	-1.0f,  1.0f,  1.0f,	-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,	 1.0f, -1.0f,  1.0f,	 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,	 1.0f,  1.0f, -1.0f,	 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,	-1.0f,  1.0f,  1.0f,	 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,	 1.0f, -1.0f,  1.0f,	-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,	 1.0f,  1.0f, -1.0f,	 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,	-1.0f,  1.0f,  1.0f,	-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,	-1.0f, -1.0f,  1.0f,	 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,	-1.0f, -1.0f,  1.0f,	 1.0f, -1.0f,  1.0f
	};
	
	// creates and binds the VAO and VBO.
	glGenVertexArrays(1, &_skybox_VAO_ID);
	glGenBuffers(1, &_skybox_VBO_ID);
	glBindVertexArray(_skybox_VAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, _skybox_VBO_ID);

	// transfers data to video card.
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), &VERTICES, GL_STATIC_DRAW);

	// specifies to the video card how the vertices should be interpreted.
	void* ptr = (void*)0;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), ptr);

	// disconnects the VAO and VBO from the system.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Camera &env::get_camera(int index) {
	if (index >= 0)
		return _cameras.at(index);
	return _cameras.back();
}

size_t env::create_camera() {
	_cameras.push_back(Camera());
	return _cameras.size() - 1;
}

void env::activate_MSAA(uint8_t n_multisamples) {
	_screenbuffer.set_n_samples(n_multisamples);
	int width, height;
	glfwGetWindowSize(_window->get_GLFWwindow(), &width, &height);
	_screenbuffer_size_callback(_window->get_GLFWwindow(), width, height);
	glfwSetFramebufferSizeCallback(
		_window->get_GLFWwindow(), _screenbuffer_size_callback
	);
}

void env::draw_skybox(
	const glm::mat4 &cam_skybox_mat, GLuint cubemap_ID
) {
	draw_skybox(_default_skybox_shader, cam_skybox_mat, cubemap_ID);
}

void env::draw_skybox(
	const SkyboxShader &skybox_shader, 
	const glm::mat4 &cam_skybox_mat, 
	GLuint cubemap_ID
) {
	skybox_shader.use();
	skybox_shader.set_PV_mat_4fv(cam_skybox_mat);
	glActiveTexture(GL_TEXTURE0 + Material::MAP_TYPE::SKYBOX);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_ID);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(_skybox_VAO_ID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	glBindVertexArray(0);
}

void env::poll_events_and_update_delta() {
	glfwPollEvents();
	gu::Delta::update();
}

// clears the default buffer and 
// then binds the <_screenbuffer> so that its image buffer is being drawn to.
void env::clear_window_and_screenbuffer() {
	if (_screenbuffer.is_used())
		_screenbuffer.bind_and_clear(_clear_color);
	else
		_window->clear();
}

void env::display_frame() {
	display_frame(_default_screen_shader);
}

// processes the <_screenbuffer> for an output frame image (texture),
// draws the Screenbuffer display with the output texture as its bound texture
// to the Screenbuffer's final screen texture, and then 
// swaps buffers with the default buffer to display the frame to the Window.
void env::display_frame(const ScreenShader &screen_shader) {
	_window->reset_viewport();
	
	if (_screenbuffer.is_used()) {
		_blit_frame_to_buffer();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		screen_shader.use();
		glBindVertexArray(_screen_display_VAO_ID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _screenbuffer.get_screen_ID());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	#if defined(_WIN32)
	DwmFlush(); // prevents microstutter on Windows
	#endif
	glfwSwapBuffers(_window->get_GLFWwindow());
}

void env::_screenbuffer_size_callback(
	GLFWwindow *window, int width, int height
) {
	if (width <= 0 or height <= 0)
		return;
	
	if (_screenbuffer.is_used())
		_screenbuffer.create(width, height);

	for (int i = 0; i < _cameras.size(); ++i)
		get_camera(i).framebuffer_size_callback(width, height);
}

void env::_blit_frame_to_buffer() {
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_BACK, GL_FILL);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _screenbuffer.get_image_ID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _screenbuffer.get_inter_ID());
	const GLsizei &w = _screenbuffer.get_width();
	const GLsizei &h = _screenbuffer.get_height();
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
} // namespace gu