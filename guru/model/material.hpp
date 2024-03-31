/**
 * material.hpp
 * ---
 * this file defines the Material class,
 * which holds various OpenGL texture IDs
 * to represent different maps (diffuse, normal, etc.)
 *
 * ---
 * under a MTL file, these keywords will look like:
 * map_Kd				diffuse.png
 * map_Kn or map_Bump	normal.png
 * map_Ks				specular.png
 * map_Ns				roughness.png
 * map_d				displacement.png (technically for transparency)
 * map_Ke				emissive.png
 * 
 * under an FBX file, these keywords may look like:
 * DiffuseTexture or DiffuseColor	diffuse.png
 * NormalMap or BumpMap				normal.png
 * SpecularMap						specular.png
 * RoughnessMap						roughness.png
 * DisplacementMap					displacement.png
 * EmissiveMap						emissive.png
 */ 
/*
 * under a DAE file, the maps may be defined as:
 *	<library_images>
 *		<image id="diffuse_image" name="diffuse_image">
 *			<init_from>diffuse.png</init_from>
 *		</image>
 * 
 *		<image id="normal_image" name="normal_image">
 *			<init_from>normal.png</init_from>
 *		</image>
 * 
 *		...
 * 
 *		<image id="emissive_image" name="emissive_image">
 *			<init_from>emissive.png</init_from>
 *		</image>
 *	</library_images>
 * 
 *	<library_effects>
 *		<effect id="diffuse_effect" name="diffuse_effect">
 *			<profile_COMMON>
 *				<technique sid="common">
 *					<phong>
 *						<diffuse>
 *							<texture texture="diffuse_image" texcoord="UVMap"/>
 *						</diffuse>
 *					</phong>
 *				</technique>
 *			</profile_COMMON>
 *		</effect>
 * 
 *		<effect id="normal_effect" name="normal_effect">
 *			...
 *						<normal>
 *							<texture texture="normal_image" texcoord="UVMap"/>
 *						</normal>
 *			...
 *		</effect>
 * 
 * 		<effect id="specular_effect" name="specular_effect">
 *			...
 *						<specular>
 *							...
 *						</specular>
 *			...
 *		</effect>
 * 
 *		<effect id="roughness_effect" name="roughness_effect">
 *			...
 *						<shininess>
 *							...
 *						</shininess>
 *			...
 *		</effect>
 * 
 *		<effect id="displacement_effect" name="displacement_effect">
 *			...
 *						<bump>
 *							...
 *						</bump>
 *			...
 *		</effect>
 * 
 *		<effect id="emissive_effect" name="emissive_effect">
 *			...
 *						<emission>
 *							...
 *						</emission>
 *			...
 *		</effect>
 *	</library_effects>
 * 
 *	<library_materials>
 *		<material id="diffuse_material" name="diffuse_material">
 *			<instance_effect url="#diffuse_effect"/>
 *		</material>
 * 
 *		...
 * 
 *		<material id="emissive_material" name="emissive_material">
 *			<instance_effect url="#emissive_effect"/>
 *		</material>
 *	</library_materials>
 * 
 *	<scene>
 *		<instance_visual_scene url="#default_scene"/>
 *	</scene>
 */
/*
 * if assimp fails to find these embeddings, then the program will try to 
 * load the map from the diffuse texture's name, 
 * with an underscore and the map type name added before the extension.
 */
#pragma once
#include <filesystem>
#include <string>
#include <glad/gl.h>
#include "../image/color.hpp"

namespace gu {
class Material {
public:
	static const uint8_t N_MAP_TYPES = 6;
private:
	static const std::string MAP_TYPES[N_MAP_TYPES];
	static const Color DEFAULT_COLORS[N_MAP_TYPES];
	GLuint _texture_IDs[N_MAP_TYPES];
	bool _map_loaded[N_MAP_TYPES];
	std::filesystem::path _diffuse_path;

public:
	// dtor. tells texture list to handle these textures no longer being used.
	~Material();
	inline const std::filesystem::path &diffuse_path() const { 
		return _diffuse_path; 
	}
	inline const GLuint &diffuse_ID() const { return _texture_IDs[0]; }
	inline const GLuint &normal_ID() const { return _texture_IDs[1]; }
	inline const GLuint &specular_ID() const { return _texture_IDs[2]; }
	inline const GLuint &roughness_ID() const { return _texture_IDs[3]; }
	inline const GLuint &displacement_ID() const { return _texture_IDs[4]; }
	inline const GLuint &reflective_ID() const { return _texture_IDs[5]; }
	
	// loads textures from a given image paths. 
	// if the paths to other texture maps aren't given,
	// then the program tries to load from the same image path 
	// as the diffuse texture, but with an underscore
	// and map type inserted before the file extension.
	void load_textures(const std::filesystem::path *paths);
};
} // namespace gu