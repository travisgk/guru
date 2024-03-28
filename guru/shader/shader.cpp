#include "shader.hpp"
#include <fstream>
#include <iostream>

// loads text from the file located at <path> into the given string <received>.
static bool load_text_from_file(
	std::string& received, const std::filesystem::path& path
) {
	std::ifstream buffer;
	buffer.exceptions(std::ifstream::badbit);

	try {
		buffer.open(path);
		std::stringstream stream;
		stream << buffer.rdbuf();
		buffer.close();
		received = stream.str();
		return true;
	}
	catch (const std::ifstream::failure& e) {
		std::cerr 
			<< "Could not load text from " << path
			<< ". Error:" << e.what() << std::endl;
		return false;
	}
}

// returns the OpenGL ID for the built part of shader
// created from the given <src> given the <shader_type>.
static GLuint build_shader_part(GLenum shader_type, const char* src) {
	// builds from source.
	GLuint shader_ID = glCreateShader(shader_type);
	glShaderSource(shader_ID, 1, &src, nullptr);
	glCompileShader(shader_ID);

	// checks if the build was successful.
	GLint build_is_successful;
	glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &build_is_successful);
	return build_is_successful ? shader_ID : 0;
}

namespace gu {
Shader::~Shader() {
	if (_program_ID != 0)
		glDeleteProgram(_program_ID);
}

bool Shader::build_from_files(
	const std::filesystem::path& v_shader_path,
	const std::filesystem::path& f_shader_path
) {
	std::string v_shader_src, f_shader_src;
	load_text_from_file(v_shader_src, v_shader_path);
	load_text_from_file(f_shader_src, f_shader_path);

	bool build_is_successful = build_from_source(
		v_shader_src.c_str(), f_shader_src.c_str()
	);
	if (not build_is_successful)
		std::cerr 
			<< "Program files: " << v_shader_path
			<< ", " << f_shader_path << std::endl;
	return build_is_successful;
}

bool Shader::build_from_source(
	const char *v_shader_src, const char *f_shader_src
) {
	if (_program_ID != 0)
		glDeleteProgram(_program_ID);

	// builds the vertex shader.
	GLuint v_shader = build_shader_part(GL_VERTEX_SHADER, v_shader_src);
	if (v_shader == 0) {
		std::cerr 
			<< "The vertex shader part failed to build." 
			<< std::endl;
		return false;
	}

	// builds the fragment shader.
	GLuint f_shader = build_shader_part(GL_FRAGMENT_SHADER, f_shader_src);
	if (f_shader == 0) {
		std::cerr 
			<< "The fragment shader part failed to build." 
			<< std::endl;
		return false;
	}

	// builds the main program.
	_program_ID = glCreateProgram();
	glAttachShader(_program_ID, v_shader);
	glAttachShader(_program_ID, f_shader);
	glLinkProgram(_program_ID);
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	// checks if the build was successful.
	GLint build_is_successful;
	glGetProgramiv(_program_ID, GL_LINK_STATUS, &build_is_successful);
	if (not build_is_successful) {
		GLchar info_log[512];
		glGetProgramInfoLog(_program_ID, 512, nullptr, info_log);
		std::cerr 
			<< "The shader program could not be linked:\n"
			<< info_log << std::endl;
		return false;
	}
	_init();
	return true;
}
} // namespace gu