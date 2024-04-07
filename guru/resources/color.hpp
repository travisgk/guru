/**
 * color.hpp
 * ---
 * this file defines the Color struct, an RGB color with an alpha component.
 * these channels range 0.0 to 1.0.
 * 
 */

#pragma once
#include <string>

namespace gu {
struct Color {
public:
	float r, g, b, a;

	// ctor. sets channel values.
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

	// returns a Color that is a transition between <this> Color
	// and the given Color <c>, where <scale> indicates
	// how far the returned Color will be transitioned from <this> to <c>.
	Color interpolate(const Color &c, const float &scale) const;

	// returns a string of the RGBA channels ranging from 0.0 to 1.0.
	std::string to_str() const;

	// returns a string of the RGBA channels ranging from 0 to 255.
	std::string to_str_255() const;
};
}