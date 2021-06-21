#include "MovementSystem.h"

void MovementSystem::add_entity(EntityID id) {
	this->entities.push_back(id);
}

void MovementSystem::update(std::vector<PositionComponent>* positionComponents, std::vector<MovementComponent>* movementComponents, float delta_s) {
	for (EntityID id : this->entities) {
		positionComponents->at(id) += movementComponents->at(id).velocity * delta_s;
	}
}
