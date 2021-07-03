#pragma once

#include <vector>
#include "EntityComponents.h"
#include <string>
#include <json.hpp>

enum EntityCreateFlags {
	Light = 1 << 0
};

struct EntityCreateInfo {
	std::string name;
	uint64_t flags;

	PositionComponent positionComponent;
	ModelComponent modelComponent;
	MovementComponent movementComponent;
	LightComponent lightComponent;
};

class EntityManager {
	std::vector<EntityID> usedIDs;
	std::vector<std::string> entityNames;
	std::vector<PositionComponent> positions;
	std::vector<ModelComponent> models;
	std::vector<MovementComponent> movements;
	std::vector<LightComponent> lightComponents;

public:
	EntityID addEntity(EntityCreateInfo* createInfo);

	void saveEntities(nlohmann::json* j);

	std::vector<EntityID>* getUsedIDs();
	std::vector<std::string>* getNames();
	std::vector<ModelComponent>* getModelComponents();
	std::vector<PositionComponent>* getPositionComponents();
	std::vector<MovementComponent>* getMovementComponents();
	std::vector<LightComponent>* getLightComponents();
};