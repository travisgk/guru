#include "color.hpp"
#include <sstream>
#include <iomanip>

namespace gu {
Color::Color(
	const float &red,
	const float &green,
	const float &blue,
	const float &alpha
) {
	_r = red;
	_g = green;
	_b = blue;
	_a = alpha;
}

Color Color::interpolate(const Color &c, const float &scale) const {
	float d_r = c.r() - _r;
	float d_g = c.g() - _g;
	float d_b = c.b() - _b;
	float d_a = c.a() - _a;
	return Color(
		_r + d_r * scale,
		_g + d_g * scale,
		_b + d_b * scale,
		_a + d_a * scale
	);
}

std::string Color::to_str() const {
	std::ostringstream stream;
	stream 
		<< '('
		<< std::fixed << std::setprecision(3) << _r << "," 
		<< std::fixed << std::setprecision(3) << _g << "," 
		<< std::fixed << std::setprecision(3) << _b << ","
		<< std::fixed << std::setprecision(3) << _a << ')';
	return stream.str();
}

std::string Color::to_str_255() const {
	std::ostringstream stream;
	stream 
		<< '('
		<< std::to_string(static_cast<uint8_t>(255.0f * _r)) << ","
		<< std::to_string(static_cast<uint8_t>(255.0f * _g)) << ","
		<< std::to_string(static_cast<uint8_t>(255.0f * _b)) << ","
		<< std::to_string(static_cast<uint8_t>(255.0f * _a)) << ')';
	return stream.str();
}
} // namespace gu