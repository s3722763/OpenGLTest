#include "Entities.h"

EntityID EntityManager::addEntity(EntityCreateInfo* createInfo) {
	EntityID id = this->entityNames.size();

	this->usedIDs.push_back(id);
	this->entityNames.push_back(createInfo->name);
	this->positions.push_back(createInfo->positionComponent);
	this->models.push_back(createInfo->modelComponent);
	this->movements.push_back(createInfo->movementComponent);

	return id;
}

void EntityManager::saveEntities(nlohmann::json* j) {
	auto entity_array = nlohmann::json::array();

	for (EntityID id : this->usedIDs) {
		entity_array.push_back({
		{
			"position", {
				{"x", this->positions.at(id).x},
				{"y", this->positions.at(id).y},
				{"z", this->positions.at(id).z}
			},
		}
		});
	}

	(*j)["entities"] = entity_array;
}

std::vector<EntityID>* EntityManager::getUsedIDs() {
	return &this->usedIDs;
}

std::vector<std::string>* EntityManager::getNames() {
	return &this->entityNames;
}

std::vector<ModelComponent>* EntityManager::getModelComponenets() {
	return &this->models;
}

std::vector<PositionComponent>* EntityManager::getPositionComponents() {
	return &this->positions;
}

std::vector<MovementComponent>* EntityManager::getMovementComponents() {
	return &this->movements;
}
