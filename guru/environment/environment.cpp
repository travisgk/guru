#include "environment.hpp"
#include "../system/settings.hpp"

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

void env::reset(Window &window) {
	// sets up the <_window> and <_cameras>.
	_window = &window;
	set_clear_color(gu::Color(0.3f, 0.3f, 0.3f));
	_create_screen_display();
	_create_skybox();
	create_camera();

	// sets up the <_screen_buffer>.
	int width, height;
	glfwGetWindowSize(_window->get_GLFWwindow(), &width, &height);
	_screenbuffer_size_callback(_window->get_GLFWwindow(), width, height);
	glfwSetFramebufferSizeCallback(
		_window->get_GLFWwindow(), _screenbuffer_size_callback
	);

	// builds the shaders.
	_screen_shader.build_from_files(
		_screen_shader_v_shader_path, _screen_shader_f_shader_path
	);
	_skybox_shader.build_from_files(
		_skybox_shader_v_shader_path, _skybox_shader_f_shader_path
	);
}

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

Camera &env::get_camera(int index) {
	if (index >= 0)
		return *_cameras.at(index);
	return *_cameras[_cameras.size() - 1];
}

void env::draw_skybox(
	const glm::mat4 &camera_projview_mat, GLuint cubemap_ID
) {
	_skybox_shader.use();
	_skybox_shader.set_projview_mat(camera_projview_mat);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(_skybox_VAO_ID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	glBindVertexArray(0);
}

void env::update_delta_and_poll_events() {
	gu::maintain_fps();
	gu::Delta::update();
	glfwPollEvents();
}

void env::clear_window_and_screenbuffer() {
	_window->clear();
	_screenbuffer.bind_and_clear(_clear_color);
	glfwSwapInterval(gu::Settings::using_vsync());
}

void env::display_frame() {
	_window->reset_viewport();
	_display_frame();
	glfwSwapBuffers(_window->get_GLFWwindow());
}

void env::_screenbuffer_size_callback(
	GLFWwindow *window, int width, int height
) {
	// UNCERTAIN: glViewport(0, 0, width, height);
	if (width <= 0 or height <= 0)
		return;

	_screenbuffer.create(width, height);
	for (int i = 0; i < _cameras.size(); ++i)
		get_camera(i).framebuffer_size_callback(width, height);
}

void env::_update_everything() {
	for (int i = 0; i < _cameras.size(); ++i)
		get_camera(i).update();
}

void env::_display_frame() {
	glDisable(GL_CULL_FACE);
	_screen_shader.use();
	glPolygonMode(GL_BACK, GL_FILL);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _screenbuffer.image_ID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _screenbuffer.inter_ID());
	const GLsizei &w = _screenbuffer.width();
	const GLsizei &h = _screenbuffer.height();
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0 );
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0 );
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(_screen_display_VAO_ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _screenbuffer.screen_ID());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace gu