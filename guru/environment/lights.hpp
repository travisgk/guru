#pragma once
#include "../resources/color.hpp"
#include "../mathmatics/quat_point.hpp"

namespace {
class LightColor : public gu::Color {
private:
	bool _is_new = true;

public:
LightColor(
	const float &red,
	const float &green,
	const float &blue,
	const float &alpha = 1.0f
) : gu::Color(red, green, blue, alpha) {}

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
	_is_new = true;
}
inline bool needs_GL_update() const { return _is_new; }
inline void set_as_GL_updated() { _is_new = false; }
};

/**
 * LightColors
 * ---
 * this class defines the Color properties of a light source.
 * 
 */
class LightColors {
protected:
	LightColor _diffuse = LightColor(1.0f, 1.0f, 1.0f);
	LightColor _specular = LightColor(1.0f, 1.0f, 1.0f);

	// instances of this base class cannot be directly created.
	inline LightColors() {}
public:
	inline LightColor &diffuse() { return _diffuse; }
	inline LightColor &specular() { return _specular; }
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
		
public:
	inline FloatUniform(const float &value, bool &external_update_indicator) 
		: _value(value), 
		  _external_update_indicator(&external_update_indicator)
	{}
	const float &value() const { return _value; }
	bool needs_GL_update() const { return _needs_GL_update; }
	inline void set(const float &value) {
		_value = value;
		_needs_GL_update = true;
		*_external_update_indicator = true;
	}
	inline void set_as_GL_updated() { 
		_needs_GL_update = false; 
		*_external_update_indicator = false;
	}
};

/**
 * LightAttenuationFactors
 * ---
 * this class defines the factors for light attenuation.
 *
 */
class LightAttenuationFactors {
protected:
	bool _attenuation_was_changed = true;
	FloatUniform _constant = FloatUniform(1.0f, _attenuation_was_changed);
	FloatUniform _linear = FloatUniform(0.09f, _attenuation_was_changed);
	FloatUniform _quadratic = FloatUniform(0.032f, _attenuation_was_changed);

	// instances of this base class cannot be directly created.
	inline LightAttenuationFactors() {}

public:
	inline bool attenuation_was_changed() const { return _attenuation_was_changed; }
	inline FloatUniform &constant() { return _constant; }
	inline FloatUniform &linear() { return _linear; }
	inline FloatUniform &quadratic() { return _quadratic; }
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
class PointLight 
	: public LightColors, 
	  public LightAttenuationFactors,
	  public Point
{
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
class SpotLight 
	: public LightColors, 
	  public LightAttenuationFactors,
	  public QuatPoint
{
protected:
	bool _cutoff_was_changed = true;
	FloatUniform _inner_cutoff = FloatUniform(
		glm::cos(glm::radians(7.0f)), _cutoff_was_changed
	);
	FloatUniform _outer_cutoff = FloatUniform(
		glm::cos(glm::radians(17.0f)), _cutoff_was_changed
	);
	bool _direction_needs_GL_update = true;
	bool _position_needs_GL_update = true;

public:
	inline FloatUniform &inner_cutoff() { return _inner_cutoff; }
	inline FloatUniform &outer_cutoff() { return _outer_cutoff; }
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