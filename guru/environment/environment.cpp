#include "environment.hpp"
#include "../system/settings.hpp"
#include "../environment/model.hpp"
#include <iostream>

namespace gu {
Window *env::_window;
GLuint env::_screen_display_VAO_ID = 0;
GLuint env::_screen_display_VBO_ID = 0;
GLuint env::_skybox_VAO_ID = 0;
GLuint env::_skybox_VBO_ID = 0;
std::vector<std::shared_ptr<Camera>> env::_cameras;
Screenbuffer env::_screenbuffer;
Color env::_clear_color = gu::Color(0.3f, 0.3f, 0.3f);
std::filesystem::path env::_screen_shader_v_shader_path = (
	"guru/shader/default_glsl/_screen_shader.v_shader"
);
std::filesystem::path env::_screen_shader_f_shader_path = (
	"guru/shader/default_glsl/_screen_shader.f_shader"
);
std::filesystem::path env::_skybox_shader_v_shader_path = (
	"guru/shader/default_glsl/_skybox_shader.v_shader"
);
std::filesystem::path env::_skybox_shader_f_shader_path = (
	"guru/shader/default_glsl/_skybox_shader.f_shader"
);
ScreenShader env::_screen_shader;
SkyboxShader env::_skybox_shader;

bool init_GLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cerr << "GLFW failed to initialize.\n";
		return false;
	}
	glfwWindowHint(
		GLFW_CONTEXT_VERSION_MAJOR, gu::Settings::OPENGL_VERSION_MAJOR
	);
	glfwWindowHint(
		GLFW_CONTEXT_VERSION_MINOR, gu::Settings::OPENGL_VERSION_MINOR
	);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4); // adds a bit more MSAA on top
	return true;
}

bool init_glad() {
	if (not gladLoadGL(GLADloadfunc(glfwGetProcAddress))) {
		std::cerr << "glad failed to initialize." << std::endl;
		return false;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);
	return true;
}

void terminate() {
	model_res_list.deallocate();
	material_list.deallocate();
	texture_list.deallocate();
	glfwTerminate();
}

env::~env() {
	if (_screen_display_VBO_ID != 0)
		glDeleteBuffers(1, &_screen_display_VBO_ID);
	if (_screen_display_VAO_ID != 0)
		glDeleteVertexArrays(1, &_screen_display_VAO_ID);
	_screen_display_VBO_ID = 0;
	_screen_display_VAO_ID = 0;

	if (_skybox_VBO_ID != 0)
		glDeleteBuffers(1, &_skybox_VBO_ID);
	if (_skybox_VAO_ID != 0)
		glDeleteVertexArrays(1, &_skybox_VAO_ID);
	_skybox_VBO_ID = 0;
	_skybox_VAO_ID = 0;
}

// sets up the <_window> and <_cameras>,
// sets up the <_screenbuffer>,
// and builds the shaders.
void env::reset(Window &window) {
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

	_screen_shader.build_from_files(
		_screen_shader_v_shader_path, _screen_shader_f_shader_path
	);
	_skybox_shader.build_from_files(
		_skybox_shader_v_shader_path, _skybox_shader_f_shader_path
	);
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
		return *_cameras.at(index);
	return *_cameras.back();
}

size_t env::create_camera() {
	std::shared_ptr<Camera> new_camera = std::make_shared<Camera>();
	_cameras.push_back(new_camera);
	return _cameras.size() - 1;
}

void env::set_screen_shader_paths(
	const std::filesystem::path &v_shader_path,
	const std::filesystem::path &f_shader_path
) {
	_screen_shader_v_shader_path = v_shader_path;
	_screen_shader_f_shader_path = f_shader_path;
}

void env::set_skybox_shader_paths(
	const std::filesystem::path &v_shader_path,
	const std::filesystem::path &f_shader_path
) {
	_skybox_shader_v_shader_path = v_shader_path;
	_skybox_shader_f_shader_path = f_shader_path;
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
	const glm::mat4 &camera_projview_mat, GLuint cubemap_ID
) {
	_skybox_shader.use();
	_skybox_shader.set_PV_mat_4fv(camera_projview_mat);
	glActiveTexture(GL_TEXTURE0 + Material::MAP_TYPE::SKYBOX);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_ID);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(_skybox_VAO_ID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	glBindVertexArray(0);
}

void env::update_delta_and_poll_events() {
	gu::Delta::maintain_fps();
	gu::Delta::update();
	glfwPollEvents();
}

// clears the default buffer and 
// then binds the <_screenbuffer> so that its image buffer is being drawn to.
void env::clear_window_and_screenbuffer() {
	

	if (_screenbuffer.is_used()) {
		_window->clear();
		_screenbuffer.bind_and_clear(_clear_color);
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		_window->clear();
	}
	
	glfwSwapInterval(gu::Settings::using_vsync());
}

// processes the <_screenbuffer> for an output frame image (texture),
// draws the Screenbuffer display with the output texture as its bound texture
// to the Screenbuffer's final screen texture, and then 
// swaps buffers with the default buffer to display the frame to the Window.
void env::display_frame() {
	_window->reset_viewport();
	
	if (_screenbuffer.is_used()) {
		_blit_frame_to_buffer();
		glBindVertexArray(_screen_display_VAO_ID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _screenbuffer.screen_ID());
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glfwSwapBuffers(_window->get_GLFWwindow());
}

void env::_screenbuffer_size_callback(
	GLFWwindow *window, int width, int height
) {
	// UNCERTAIN: glViewport(0, 0, width, height);
	if (width <= 0 or height <= 0)
		return;
	
	if (_screenbuffer.is_used())
		_screenbuffer.create(width, height);

	for (int i = 0; i < _cameras.size(); ++i)
		get_camera(i).framebuffer_size_callback(width, height);
}

void env::_update_everything() {
	for (int i = 0; i < _cameras.size(); ++i)
		get_camera(i).update();
}

void env::_blit_frame_to_buffer() {
	_screen_shader.use();
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_BACK, GL_FILL);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _screenbuffer.image_ID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _screenbuffer.inter_ID());
	const GLsizei &w = _screenbuffer.width();
	const GLsizei &h = _screenbuffer.height();
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
}
} // namespace gu