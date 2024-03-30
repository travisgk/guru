#include "color.hpp"
#include <sstream>
#include <iomanip>

namespace gu {
Color::Color(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

Color Color::interpolate(const Color &c, const float &scale) const {
	float d_r = c.r - r;
	float d_g = c.g - g;
	float d_b = c.b - b;
	float d_a = c.a - a;
	return Color(
		r + d_r * scale,
		g + d_g * scale,
		b + d_b * scale,
		a + d_a * scale
	);
}

std::string Color::to_str() const {
	std::ostringstream stream;
	stream 
		<< '('
		<< std::fixed << std::setprecision(3) << r << ", " 
		<< std::fixed << std::setprecision(3) << g << ", " 
		<< std::fixed << std::setprecision(3) << b << ", "
		<< std::fixed << std::setprecision(3) << a << ')';
	return stream.str();
}

std::string Color::to_str_255() const {
	std::ostringstream stream;
	stream 
		<< '('
		<< std::to_string(static_cast<uint8_t>(255.0f * r)) << ", "
		<< std::to_string(static_cast<uint8_t>(255.0f * g)) << ", "
		<< std::to_string(static_cast<uint8_t>(255.0f * b)) << ", "
		<< std::to_string(static_cast<uint8_t>(255.0f * a)) << ')';
	return stream.str();
}
} // namespace gu