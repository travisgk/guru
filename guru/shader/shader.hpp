/**
 * shader.hpp
 * ---
 * this file defines the Shader class for building
 * and interfacing with an OpenGL shader program.
 * 
 */

#pragma once
#include <filesystem>
#include <glad/gl.h>

namespace gu {
class Shader {
protected:
GLuint _program_ID = 0;

public:
	// deconstructor. deletes the GL program if it exists.
	~Shader();

public:
	inline void use() { glUseProgram(_program_ID); }

	// returns true if the shader program was successfully built.
	virtual bool build_from_files(
		const std::filesystem::path& v_shader_path,
		const std::filesystem::path& f_shader_path
	);

	// returns true if the shader program was successfully built.
	// builds the shader program from the given 
	// vertex shader source code (<v_shader_src>) 
	// and the given fragment shader source code (<f_shader_src>).
	virtual bool build_from_source(
		const char *v_shader_src, const char *f_shader_src 
	);

protected:
	// this method is run once the Shader is built.
	inline virtual void _init() { _config_uniform_IDs(); }

	// sets the class's contained uniform IDs 
	// by searching for them in the code.
	inline virtual void _config_uniform_IDs() {}
};
} // namespace gu