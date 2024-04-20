#pragma once

#define NIFLIB_STATIC_LINK
#include <niflib.h>
#include <obj/NiObject.h>
#include "../../glm/ext.hpp"

namespace Niflib
{
	NifInfo ReadHeaderInfo(std::istream &in);

	bool TryExtractMesh(NiObject *obj, glm::mat4 const &nifWorld, std::vector<glm::vec3> &vertices, std::vector<uint16_t> &indices);

	inline glm::mat4 ToMat4(Matrix33 const &m)
	{
		return glm::mat4(
			m.rows[0][0], m.rows[0][1], m.rows[0][2], 0.f,
			m.rows[1][0], m.rows[1][1], m.rows[1][2], 0.f,
			m.rows[2][0], m.rows[2][1], m.rows[2][2], 0.f,
			0.f,                   0.f,          0.f, 1.f
		);
	}
	inline glm::mat4 ToMat4(Matrix44 const &m)
	{
		return glm::mat4(
			m.rows[0][0], m.rows[0][1], m.rows[0][2], m.rows[0][3],
			m.rows[1][0], m.rows[1][1], m.rows[1][2], m.rows[1][3],
			m.rows[2][0], m.rows[2][1], m.rows[2][2], m.rows[2][3],
			m.rows[3][0], m.rows[3][1], m.rows[3][2], m.rows[3][3]
		);
	}
	inline glm::vec3 ToVec3(Vector3 const &v)
	{
		return glm::vec3(v.x, v.y, v.z);
	}
	inline glm::vec4 ToVec4(Vector3 const &v)
	{
		return glm::vec4(v.x, v.y, v.z, 1);
	}
}
