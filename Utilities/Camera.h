#pragma once
#include <glm/vec3.hpp>

struct Camera {
	glm::vec3 Position;
	glm::vec3 LookAt;
	glm::vec3 Up;
};