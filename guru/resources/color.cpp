#include "color.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace gu {
Color::Color(
	const float &red,
	const float &green,
	const float &blue,
	const float &alpha
) {
	set(red, green, blue, alpha);
}

void Color::set(
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

Color Color::interpolate(
	const Color &color_a, const Color &color_b, const float &scale
) {
	float d_r = color_b.get_r() - color_a.get_r();
	float d_g = color_b.get_g() - color_a.get_g();
	float d_b = color_b.get_b() - color_a.get_b();
	float d_a = color_b.get_a() - color_a.get_a();
	return Color(
		color_a.get_r() + d_r * scale,
		color_a.get_g() + d_g * scale,
		color_a.get_b() + d_b * scale,
		color_a.get_a() + d_a * scale
	);
}

glm::vec3 Color::to_HSV(const Color &c) {
	float max_val = std::max(c.get_r(), std::max(c.get_g(), c.get_b()));
	float min_val = std::min(c.get_r(), std::min(c.get_g(), c.get_b()));
	float delta = max_val - min_val;
	glm::vec3 hsv(0.0f);

	// hue.
	if (delta < 1e-5f)
		hsv[0] = 0.0f;
	else if (max_val == c.get_r())
		hsv[0] = 60.0f * (fmod(((c.get_g() - c.get_b()) / delta), 6.0f));
	else if (max_val == c.get_g())
		hsv[0] = 60.0f * (((c.get_b() - c.get_r()) / delta) + 2.0f);
	else
		hsv[0] = 60.0f * (((c.get_r() - c.get_g()) / delta) + 4.0f);

	// saturation.
	if (max_val < 1e-5f)
		hsv[1] = 0.0f;
	else
		hsv[1] = delta / max_val;

	// value.
	hsv[2] = max_val;

	return hsv;
}

Color Color::from_HSV(const glm::vec3 &hsv) {
	glm::vec3 rgb(0.0f);
	if (hsv[0] < 0.0f)
		return Color(rgb);

	float hue = hsv[0] / 60.0f; // scales hue to [0, 6)
	float c = hsv[1] * hsv[2];
	float x = c * (1.0f - std::abs(fmod(hue, 2.0f) - 1.0f));
	float m = hsv[2] - c;

	if (hue < 1.0f)
		rgb = glm::vec3(c, x, 0.0f);
	else if (hue < 2.0f)
		rgb = glm::vec3(x, c, 0.0f);
	else if (hue < 3.0f)
		rgb = glm::vec3(0.0f, c, x);
	else if (hue < 4.0f)
		rgb = glm::vec3(0.0f, x, c);
	else if (hue < 5.0f)
		rgb = glm::vec3(x, 0.0f, c);
	else
		rgb = glm::vec3(c, 0.0, x);

	rgb += glm::vec3(m);
	return Color(rgb);
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