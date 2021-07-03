#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <glad/gl.h>

typedef size_t EntityID;

enum ModelFlags {
	Coloured = 1 << 0,
	Textured = 1 << 1
};

struct MeshComponent {
	GLuint IndexBufferObject;
	GLuint VertexBufferObject;
	GLuint TextureCoordBufferObject;
	GLuint NormalBufferObject;
	GLuint ColorBufferObject;
	GLuint VAO;
	GLuint textureID;

	glm::mat4x4 originalTransform;

	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> tangents;
	std::vector<glm::vec2> texcoords0;
	std::vector<glm::vec2> texcoords1;
	std::vector<glm::vec4> colors;

	uint64_t flags;
};

struct ModelComponent {
	std::vector<MeshComponent> meshes;
};

struct MovementComponent {
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

struct LightComponent {
	glm::vec4 color;
	glm::vec4 position;
};

typedef glm::vec3 PositionComponent;