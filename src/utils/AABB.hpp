#pragma once

#include "../common.hpp"

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	AABB() : min(0), max(0) {}

	explicit AABB(std::vector<glm::vec3> const &vertices) :
			min(glm::vec3(std::numeric_limits<float>::max())),
			max(glm::vec3(std::numeric_limits<float>::min())) {
		for (auto &v: vertices) {
			min.x = std::min(min.x, v.x);
			min.y = std::min(min.y, v.y);
			min.z = std::min(min.z, v.z);
			max.x = std::max(max.x, v.x);
			max.y = std::max(max.y, v.y);
			max.z = std::max(max.z, v.z);
		}
	}

	bool intersects(AABB const &other) const {
		if (max.x < other.min.x || min.x > other.max.x ||
		    max.y < other.min.y || min.y > other.max.y ||
		    max.z < other.min.z || min.z > other.max.z)
			return false;
		return true;
	}

	AABB translate(glm::vec3 const &translation) const {
		auto cpy = *this;
		cpy.min += translation;
		cpy.max += translation;
		return cpy;
	}
};
