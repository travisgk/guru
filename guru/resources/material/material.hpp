/**
 * material.hpp
 * ---
 * this file defines the Material class,
 * which holds various shared pointers to TextureInfos
 * to represent different maps (diffuse, normal, etc.)
 *
 */
/* ---
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
 *
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
#include "../texture/texture_info.hpp"
#include "../color.hpp"

namespace gu {
class Material {
public:
	/**
	 * Material::Override
	 * ---
	 * this struct specifies to a ModelResource's draw function
	 * to go to the <material_index> of the ModelResource's Materials list
	 * and temporarily replace the element with the given <material>.
	 * 
	 * this is useful for changing all the Meshes
	 * that use one particular Material.
	 * 
	 */
	struct Override {
		size_t material_index;
		std::shared_ptr<Material> material;

		inline Override(
			const size_t &material_index,
			const std::shared_ptr<Material> &material
		) : material_index(material_index), material(material) {}
	};
	enum MAP_TYPE {
		DIFFUSE = 0, 
		NORMAL, 
		DISPLACEMENT,
		ROUGHNESS, 
		AMBIENT_OCCLUSION,
		METALLIC,
		EMISSIVE, 
		SKYBOX, 
		ENUM_MAX
	};

	// matches MAP_TYPE enum.
	static const std::string MAP_TYPE_STRS[MAP_TYPE::ENUM_MAX];

	// number of loaded map types.
	static const uint8_t N_MAP_TYPES = 7;

private:
	static const Color DEFAULT_COLORS[N_MAP_TYPES]; // when no image is found
	std::shared_ptr<res::TextureInfo> _texture_infos[N_MAP_TYPES]; // maps
	bool _map_loaded[N_MAP_TYPES]; // true if map type was loaded
	bool _transparent = false; // true if diffuse has transparency

public:
	// ctor. initializes member variables.
	Material();

	// dtor. tells the global TextureList 
	// to handle these textures no longer being used.
	~Material();

	// returns the path to the image used for the diffuse texture.
	inline const std::filesystem::path &get_path() const { 
		return _texture_infos[0]->get_path(); 
	}

	// returns true if an image for a particular map type was found and loaded.
	inline bool uses_map(uint8_t map_type) const {
		return (map_type < N_MAP_TYPES and _map_loaded[map_type]);
	}

	// returns true if the center pixel of the diffuse texture is transparent.
	inline bool is_transparent() const { return _transparent; }
	
	// loads textures based on the given diffuse image path.
	void load_textures(const std::filesystem::path &diffuse_path);

	// loads textures from the given image paths. 
	// if the paths to other texture maps aren't given,
	// then the program tries to load from the same image path 
	// as the diffuse texture, but with an underscore
	// and map type inserted before the file extension.
	// if a file path isn't found, some solid color texture
	// will be created by the program.
	void load_textures(const std::filesystem::path *paths);

	// binds the textures for OpenGL rendering.
	void bind_to_GL() const;
};
} // namespace gu