#pragma once

#include <vector>
#include "EntityComponents.h"
#include <string>
#include <json.hpp>

struct EntityCreateInfo {
	std::string name;
	PositionComponent positionComponent;
	ModelComponent modelComponent;
	MovementComponent movementComponent;
};

class EntityManager {
	std::vector<EntityID> usedIDs;
	std::vector<std::string> entityNames;
	std::vector<PositionComponent> positions;
	std::vector<ModelComponent> models;
	std::vector<MovementComponent> movements;

public:
	EntityID addEntity(EntityCreateInfo* createInfo);

	void saveEntities(nlohmann::json* j);

	std::vector<EntityID>* getUsedIDs();
	std::vector<std::string>* getNames();
	std::vector<ModelComponent>* getModelComponenets();
	std::vector<PositionComponent>* getPositionComponents();
	std::vector<MovementComponent>* getMovementComponents();
};