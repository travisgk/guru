#pragma once
#include "../resources/color.hpp"
#include "../mathmatics/quat_point.hpp"

namespace {
class LightColor : public gu::Color {
private:
	bool _color_is_new = true;
	bool *_light_is_modified_indicator = nullptr;

public:
LightColor(
	const float &red,
	const float &green,
	const float &blue,
	bool light_is_modified_indicator
) : gu::Color(red, green, blue, 1.0f),
    _light_is_modified_indicator(&light_is_modified_indicator)
{}

virtual void set(
	const float &red,
	const float &green,
	const float &blue,
	const float &alpha = 1.0
) override {
	_r = red;
	_g = green;
	_b = blue;
	_a = alpha;
	_color_is_new = true;
	*_light_is_modified_indicator = true;
}
inline bool needs_GL_update() const { return _color_is_new; }
inline void set_as_GL_updated() { _color_is_new = false; }
};

/**
 * LightColors
 * ---
 * this class defines the Color properties of a light source.
 *
 */
class LightColors {
protected:
	bool _is_new = true; // true if any attribute of the Light is changed
	LightColor _diffuse = LightColor(1.0f, 1.0f, 1.0f, _is_new);
	LightColor _specular = LightColor(1.0f, 1.0f, 1.0f, _is_new);

	// instances of this base class cannot be directly created.
	inline LightColors() {}
public:
	inline LightColor &get_diffuse() { return _diffuse; }
	inline LightColor &get_specular() { return _specular; }
	inline bool needs_any_GL_update() const { return _is_new; }
	inline void set_as_entirely_GL_updated() {
		_is_new = false;
	}
};

/**
 * FloatUniform
 * ---
 * this class defines a float value that will report
 * when it's been modified.
 *
 */
class FloatUniform {
protected:
	float _value;
	bool _needs_GL_update = true;
	bool *_external_update_indicator = nullptr;
	bool *_light_is_modified_indicator = nullptr;

public:
	inline FloatUniform(
		const float &value,
		bool &external_update_indicator,
		bool &light_is_modified_indicator
	)
		: _value(value),
		  _external_update_indicator(&external_update_indicator),
		  _light_is_modified_indicator(&light_is_modified_indicator)
	{}
	const float &get_value() const { return _value; }
	bool needs_GL_update() const { return _needs_GL_update; }
	inline void set(const float &value) {
		_value = value;
		_needs_GL_update = true;
		*_external_update_indicator = true;
		*_light_is_modified_indicator = true;
	}
	inline void set_as_GL_updated() {
		_needs_GL_update = false;
	}
};

/**
 * AttenuatedLightColors
 * ---
 * this class defines a LightColors child class
 * with the factors for light attenuation.
 *
 */
class AttenuatedLightColors : public LightColors {
protected:
	bool _attenuation_needs_GL_update = true;

	FloatUniform _constant = FloatUniform(
		1.0f, _attenuation_needs_GL_update, _is_new
	);
	FloatUniform _linear = FloatUniform(
		0.09f, _attenuation_needs_GL_update, _is_new
	);
	FloatUniform _quadratic = FloatUniform(
		0.032f, _attenuation_needs_GL_update, _is_new
	);

	// instances of this base class cannot be directly created.
	inline AttenuatedLightColors() {}

public:
	inline bool attenuation_needs_GL_update() const {
		return _attenuation_needs_GL_update;
	}

	inline void set_attenuation_as_GL_updated() {
		_attenuation_needs_GL_update = false;
	}
	inline FloatUniform &get_constant() { return _constant; }
	inline FloatUniform &get_linear() { return _linear; }
	inline FloatUniform &get_quadratic() { return _quadratic; }
};
} // blank namespace

namespace gu {
/**
 * DirLight
 * ---
 * this class defines a light source that is purely directional.
 * this could be used to emulate the direction of the sunshine.
 *
 */
class DirLight : public LightColors, public Orientation {
protected:
	bool _direction_needs_GL_update = true;

public:
	inline bool direction_needs_GL_update() const {
		return _direction_needs_GL_update;
	}

	inline void set_direction_as_GL_updated() {
		_direction_needs_GL_update = false;
	}

protected:
	// this is called by any method that modifies the rotation quaternion.
	void _set_orientation_as_modified() override;
};

/**
 * PointLight
 * ---
 * this class defines a light source that is radiates from a 3D point.
 *
 */
class PointLight : public AttenuatedLightColors, public Point {
protected:
	bool _position_needs_GL_update = true;

public:
	inline bool position_needs_GL_update() const {
		return _position_needs_GL_update;
	}
	inline void set_position_as_GL_updated() {
		_position_needs_GL_update = false;
	}

protected:
	// this is called by any method that modifies the position.
	void _set_position_as_modified() override;
};

/**
 * SpotLight
 * ---
 * this class defines a light source that
 * projects light from a 3D point in a particular direction.
 *
 */
class SpotLight : public AttenuatedLightColors, public QuatPoint {
protected:
	bool _cutoff_was_changed = true;
	FloatUniform _inner_cutoff = FloatUniform(
		glm::cos(glm::radians(7.0f)), _cutoff_was_changed, _is_new
	);
	FloatUniform _outer_cutoff = FloatUniform(
		glm::cos(glm::radians(17.0f)), _cutoff_was_changed, _is_new
	);
	bool _direction_needs_GL_update = true;
	bool _position_needs_GL_update = true;

public:
	inline FloatUniform &get_inner_cutoff() { return _inner_cutoff; }
	inline FloatUniform &get_outer_cutoff() { return _outer_cutoff; }
	inline bool direction_needs_GL_update() const {
		return _direction_needs_GL_update;
	}

	inline bool position_needs_GL_update() const {
		return _position_needs_GL_update;
	}

	inline void set_direction_as_GL_updated() {
		_direction_needs_GL_update = false;
	}

	inline void set_position_as_GL_updated() {
		_position_needs_GL_update = false;
	}

protected:
	// this is called by any method that modifies the rotation quaternion.
	void _set_orientation_as_modified() override;

	// this is called by any method that modifies the position.
	void _set_position_as_modified() override;
};
} // namespace gu