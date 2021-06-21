#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>
#include <glad/gl.h>

typedef size_t EntityID;

enum ModelFlags {
	Coloured = 1 << 0,
	Textured = 1 << 1
};

struct ModelComponent {
	GLuint IndexBufferObject;
	GLuint VertexBufferObject;
	GLuint TextureCoordBufferObject;
	GLuint VAO;
	GLuint textureID;

	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec3> colors;
	glm::vec3 model_position;
	uint64_t flags;
};

struct MovementComponent {
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

typedef glm::vec3 PositionComponent;