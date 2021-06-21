#pragma once
#include "../EntityComponents.h"
#include <vector>
#include "../../Utilities/Pipeline.h"
#include "../../Utilities/Camera.h"

enum LoadTextureInfoFlags {
	Texture = 1 << 0,
	Normal = 1 << 1
};

struct LoadTextureInfo {
	GLuint wrapS = GL_REPEAT;
	GLuint wrapT = GL_REPEAT;
	GLuint minFilter = GL_LINEAR;
	GLuint magFilter = GL_LINEAR;
	const char* texture_file;
	GLuint type;
	uint64_t flags;
};

class RenderSystem {
	std::vector<EntityID> entities;

	Pipeline colouredPipeline;
	Pipeline texturedPipeline;

	void load_mesh(MeshComponent* modelComponent);
	void renderMesh(glm::mat4 modelComponent, glm::mat4 projection, glm::mat4 view, MeshComponent* mesh);

public:
	void init();

	void render(std::vector<ModelComponent>* modelComponents, std::vector<PositionComponent>* positionComponents, Camera* camera, int width, int height);
	void load_model(ModelComponent* modelComponent);
	void load_texture(MeshComponent* model, LoadTextureInfo* loadTextureInfo);
	void add_model(EntityID id);
};
