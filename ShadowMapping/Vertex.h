#pragma once

#include <glm/glm.hpp>

/**
 * This structure defines a vertex data.
 */
struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec3 texCoord;

	Vertex() {}

	Vertex(const glm::vec3& pos, const glm::vec3& c, const glm::vec3& n, const glm::vec3& tex) {
		position = pos;
		color = c;
		normal = n;
		texCoord = tex;
	}
};
