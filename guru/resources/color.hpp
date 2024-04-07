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
class Color {
protected:
	float _r, _g, _b, _a;
	bool _is_new = true;

public:
	// ctor. sets channel values.
	Color(
		const float &red, 
		const float &green, 
		const float &blue, 
		const float &alpha = 1.0
	);

	inline const float &r() const { return _r; }
	inline const float &g() const { return _g; }
	inline const float &b() const { return _b; }
	inline const float &a() const { return _a; }
	inline unsigned char unsigned_char_r() const { 
		return static_cast<unsigned char>(255.0f * _r); 
	}

	inline unsigned char unsigned_char_g() const {
		return static_cast<unsigned char>(255.0f * _g);
	}

	inline unsigned char unsigned_char_b() const {
		return static_cast<unsigned char>(255.0f * _b);
	}

	inline unsigned char unsigned_char_a() const {
		return static_cast<unsigned char>(255.0f * _a);
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