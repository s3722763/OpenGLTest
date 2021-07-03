#include <vector>
#include "../../EntityComponents.h"
#include "../../../Utilities/Pipeline.h"

class LightingSystem {
private:
	GLuint uniformBufferObject;

	std::vector<EntityID> lights;
	size_t number_lights;

public:
	void addToVAO();
	void setupPipelineForLighting(Pipeline* pipeline);
	void use(std::vector<LightComponent>* components, std::vector<PositionComponent>* position);

	void addEntity(EntityID id);
};