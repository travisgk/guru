/**
 * color.hpp
 * ---
 * this file defines the Color class, an RGB color with an alpha component.
 * these channels range 0.0 to 1.0.
 * 
 */

#pragma once
#include <string>

namespace gu {
struct Color {
public:
	float r, g, b, a;

	Color(float r, float g, float b, float a = 1.0);
	inline unsigned char unsigned_char_r() const { 
		return static_cast<unsigned char>(255.0f * r); 
	}
	inline unsigned char unsigned_char_g() const {
		return static_cast<unsigned char>(255.0f * g);
	}
	inline unsigned char unsigned_char_b() const {
		return static_cast<unsigned char>(255.0f * b);
	}
	inline unsigned char unsigned_char_a() const {
		return static_cast<unsigned char>(255.0f * a);
	}
	Color interpolate(const Color& c, const float& scale) const;
	std::string to_str() const;
	std::string to_str_255() const;
};
}