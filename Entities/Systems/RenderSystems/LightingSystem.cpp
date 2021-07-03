#include "LightingSystem.h"
#include <array>
#include <iostream>

constexpr auto MAX_LIGHTS = 10;
constexpr auto UNIFORM_BINDING_POINT = 0;

void LightingSystem::addToVAO() {
	glGenBuffers(1, &this->uniformBufferObject);

	glBindBuffer(GL_UNIFORM_BUFFER, this->uniformBufferObject);
	glBufferData(GL_UNIFORM_BUFFER, MAX_LIGHTS * sizeof(LightComponent), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, UNIFORM_BINDING_POINT, this->uniformBufferObject, 0, MAX_LIGHTS * sizeof(LightComponent));
}

void LightingSystem::setupPipelineForLighting(Pipeline* pipeline) {
	pipeline->bindUniformBlock("lights", UNIFORM_BINDING_POINT);
}

// TODO: Add sorting
void LightingSystem::use(std::vector<LightComponent>* components, std::vector<PositionComponent>* position) {
	std::array<LightComponent, MAX_LIGHTS> active_lights{};

	for (int i = 0; i < MAX_LIGHTS; i++) {
		if (i == number_lights) {
			break;
		} else {
			// Update position
			size_t index = this->lights[i];

			active_lights[i] = components->at(index);
			active_lights[i].position = glm::vec4(position->at(index), 1.0);

			//std::cout << "Position: X: " << active_lights[i].position.x << "\tY: " << active_lights[i].position.y << "\tZ: " << active_lights[i].position.z << "\n";
		}
	}

	glBindBuffer(GL_UNIFORM_BUFFER, this->uniformBufferObject);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, active_lights.size() * sizeof(LightComponent), active_lights.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightingSystem::addEntity(EntityID id) {
	this->lights.push_back(id);
	number_lights += 1;
}
