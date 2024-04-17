/**
 * color.hpp
 * ---
 * this file defines the Color struct, an RGB color with an alpha component.
 * these channels range 0.0 to 1.0.
 *
 */

#pragma once
#include <string>
#include <glm/vec3.hpp>

namespace gu {
class Color {
protected:
	float _r, _g, _b, _a;

public:
	// ctor. sets channel values.
	Color(
		const float &red,
		const float &green,
		const float &blue,
		const float &alpha = 1.0f
	);

	inline Color(const glm::vec3 &rgb, const float &alpha = 1.0f) {
		Color(rgb[0], rgb[1], rgb[2], alpha);
	}

	inline glm::vec3 as_rgb() const { return glm::vec3(_r, _g, _b); }
	inline const float &get_r() const { return _r; }
	inline const float &get_g() const { return _g; }
	inline const float &get_b() const { return _b; }
	inline const float &get_a() const { return _a; }
	inline unsigned char get_unsigned_char_r() const {
		return static_cast<unsigned char>(255.0f * _r);
	}

	inline unsigned char get_unsigned_char_g() const {
		return static_cast<unsigned char>(255.0f * _g);
	}

	inline unsigned char get_unsigned_char_b() const {
		return static_cast<unsigned char>(255.0f * _b);
	}

	inline unsigned char get_unsigned_char_a() const {
		return static_cast<unsigned char>(255.0f * _a);
	}

	virtual void set(
		const float &red,
		const float &green,
		const float &blue,
		const float &alpha = 1.0
	);

	// returns a Color that is a transition between <color_a>
	// and <color_b>, where <scale> indicates
	// how far the returned Color will be transitioned from <this> to <c>.
	static Color interpolate(
		const Color &color_a, const Color &color_b, const float &scale
	);

	// returns a vector with three channels:
	// hue [0, 360), saturation [0, 1], value [0, 1].
	static glm::vec3 to_HSV(const Color &c);

	// returns a Color that's created from a given vector
	// representing hue, saturation, and value.
	static Color from_HSV(const glm::vec3 &hsv);

	// returns a string of the RGBA channels ranging [0, 1].
	std::string to_str() const;

	// returns a string of the RGBA channels ranging [0, 255].
	std::string to_str_255() const;
};
}