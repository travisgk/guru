/**
 * assimp_to_glm.hpp
 * ---
 * this file contains functions
 * for converting assimp structs into GLM structs.
 *
 */

#pragma once
#include <assimp/anim.h>
#include <glm/ext/vector_float3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/quaternion_float.hpp>

 // sets the given glm::vec2 elements by reference from the given aiVector3D.
static void set_vec2(glm::vec2 &vec, const aiVector3D &value) {
	vec.x = value.x;
	vec.y = value.y;
}

// sets the given glm::vec3 elements by reference from the given aiVector3D.
static void set_vec3(glm::vec3 &vec, const aiVector3D &value) {
	vec.x = value.x;
	vec.y = value.y;
	vec.z = value.z;
}

// sets the given glm::mat4 elements by reference from the given aiMatrix4x4.
static void set_mat4(glm::mat4 &mat, const aiMatrix4x4 &value) {
	mat[0][0] = value.a1;
	mat[1][0] = value.a2;
	mat[2][0] = value.a3;
	mat[3][0] = value.a4;

	mat[0][1] = value.b1;
	mat[1][1] = value.b2;
	mat[2][1] = value.b3;
	mat[3][1] = value.b4;

	mat[0][2] = value.c1;
	mat[1][2] = value.c2;
	mat[2][2] = value.c3;
	mat[3][2] = value.c4;

	mat[0][3] = value.d1;
	mat[1][3] = value.d2;
	mat[2][3] = value.d3;
	mat[3][3] = value.d4;
}

// sets the given glm::quat elements by reference from the given aiQuaternion.
static void set_quat(glm::quat &quat, const aiQuaternion &value) {
	quat[0] = value.w;
	quat[1] = value.x;
	quat[2] = value.y;
	quat[3] = value.z;
}