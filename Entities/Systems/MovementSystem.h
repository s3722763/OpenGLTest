#include <vector>
#include "../EntityComponents.h"

class MovementSystem {
	std::vector<EntityID> entities;

public:
	void add_entity(EntityID id);
	void update(std::vector<PositionComponent>* positionComponents, std::vector<MovementComponent>* movementComponents, float delta_s);
};