/**
 * texture_info.hpp
 * ---
 * this file defines the <TextureInfo> struct,
 * which holds an OpenGL texture ID and an image path.
 *
 */

#pragma once
#include <filesystem>
#include <glad/gl.h>

namespace gu {
namespace res {
struct TextureInfo {
private:
	const std::filesystem::path _path;

public:
	const GLuint texture_ID;

	// ctor. sets member variables.
	inline TextureInfo( 
		const std::filesystem::path &path, const GLuint &texture_ID
	) : _path(path), texture_ID(texture_ID) 
	{}

	// returns the path of the loaded image.
	inline const std::filesystem::path &path() const { 
		return _path; 
	}
};
} // namespace res
} // namespace gu