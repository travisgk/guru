/**
 * material.hpp
 * ---
 * this file defines the Material class,
 * which holds various OpenGL texture IDs
 * to represent different maps (diffuse, normal, etc.)
 *
 */

#pragma once
#include <filesystem>
#include <glad/gl.h>
#include "../image/color.hpp"

namespace gu {
class Material {
private:
	static const std::string MAP_TYPES[6];
	static const Color DEFAULT_COLORS[6];
	GLuint _texture_IDs[6];

public:
	// dtor. tells texture list to handle these textures no longer being used.
	~Material();
	inline const GLuint &diffuse_ID() const { return _texture_IDs[0]; }
	inline const GLuint &normal_ID() const { return _texture_IDs[1]; }
	inline const GLuint &specular_ID() const { return _texture_IDs[2]; }
	inline const GLuint &roughness_ID() const { return _texture_IDs[3]; }
	inline const GLuint &displacement_ID() const { return _texture_IDs[4]; }
	inline const GLuint &reflective_ID() const { return _texture_IDs[5]; }
	
	// loads textures from a given image path. other texture maps
	// are loaded from the same image path but with an underscore
	// and map type inserted before the file extension.
	void load_textures(const std::filesystem::path &path);
};
} // namespace gu