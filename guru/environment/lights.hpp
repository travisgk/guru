#pragma once
#include "../resources/color.hpp"
#include "../mathmatics/quat_point.hpp"

namespace {
/**
	* LightColors
	* ---
	* this class defines the Color properties of a light source.
	* 
	*/
class LightColors {
protected:
	gu::Color _diffuse = gu::Color(1.0f, 1.0f, 1.0f);
	gu::Color _specular = gu::Color(1.0f, 1.0f, 1.0f);

	// instances of this base class cannot be directly created.
	inline LightColors() {}
public:
	inline gu::Color &diffuse() { return _diffuse; }
	inline gu::Color &specular() { return _specular; }
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
		
public:
	inline FloatUniform(const float &value) : _value(value) {}
	const float &value() const { return _value; }
	bool needs_GL_update() const { return _needs_GL_update; }
};

/**
	* LightAttenuationFactors
	* ---
	* this class defines the factors for light attenuation.
	*
	*/
class LightAttenuationFactors {
protected:
	FloatUniform _constant = FloatUniform(1.0f);
	FloatUniform _linear = FloatUniform(0.09f);
	FloatUniform _quadratic = FloatUniform(0.032f);

	// instances of this base class cannot be directly created.
	inline LightAttenuationFactors() {}

public:
	inline const FloatUniform &constant_factor() const { return _constant; }
	inline const FloatUniform &linear_factor() const { return _linear; }
	inline const FloatUniform &quadratic_factor() const { return _quadratic; }
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
	bool _orientation_needs_GL_update = true;

public:
	inline bool orientation_needs_GL_update() const {
		return _orientation_needs_GL_update;
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
	FloatUniform _inner_cutoff = FloatUniform(glm::cos(glm::radians(7.0f)));
	FloatUniform _outer_cutoff = FloatUniform(glm::cos(glm::radians(17.0f)));
	bool _orientation_needs_GL_update = true;
	bool _position_needs_GL_update = true;

public:
	inline const FloatUniform &inner_cutoff() const { 
		return _inner_cutoff; 
	}

	inline const FloatUniform &outer_cutoff() const { 
		return _outer_cutoff; 
	}

	inline bool orientation_needs_GL_update() const {
		return _orientation_needs_GL_update;
	}

	inline bool position_needs_GL_update() const {
		return _position_needs_GL_update;
	}

	inline void set_orientation_GL_updated() { 
		_orientation_needs_GL_update = false;
	}

	inline void set_position_GL_updated() {
		_position_needs_GL_update = false;
	}

protected:
	// this is called by any method that modifies the rotation quaternion.
	void _set_orientation_as_modified() override;

	// this is called by any method that modifies the position.
	void _set_position_as_modified() override;
};
} // namespace gu