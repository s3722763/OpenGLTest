#include "RenderSystem.h"
#include "RenderSystem.h"
#include "RenderSystem.h"
#include "RenderSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "stb_image.h"
#include "../Entities.h"

void RenderSystem::init() {
	this->colouredPipeline.init("Resources/shaders/coloured.vert", "Resources/shaders/coloured.frag");
	this->texturedPipeline.init("Resources/shaders/textured.vert", "Resources/shaders/textured.frag");

	lightingSystem.setupPipelineForLighting(&this->colouredPipeline);
	lightingSystem.setupPipelineForLighting(&this->texturedPipeline);
}

void RenderSystem::render(std::vector<ModelComponent>* modelComponents, std::vector<PositionComponent>* positionComponents, std::vector<LightComponent>* lightComponents,
		Camera* camera, int width, int height) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	auto look_vector = camera->Position + camera->LookAt;

	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);
	view = glm::lookAt(camera->Position, look_vector, camera->Up);

	this->lightingSystem.use(lightComponents, positionComponents);

	glm::mat4 viewProj = projection * view;

	for (EntityID id : this->entities) {
		for (int i = 0; i < modelComponents->at(id).meshes.size(); i++) {
			//glm::mat4 modelTransform = modelComponents->at(id).meshes[i].originalTransform;
			//modelTransform = glm::scale(modelTransform, { 0.1, 0.1, 0.1 });
			glm::mat4 modelTransform = glm::mat4{ 1.0f };
			//modelTransform = glm::translate(modelTransform, positionComponents->at(id));
			this->renderMesh(modelTransform, viewProj, &modelComponents->at(id).meshes[i], camera);
		}
	}
}

void RenderSystem::renderMesh(glm::mat4 modelTransform, glm::mat4 viewProj, MeshComponent* mesh, Camera* camera) {
	if ((mesh->flags & ModelFlags::Coloured) != 0) {
		this->colouredPipeline.use();
		this->colouredPipeline.setMatrix4x4Uniform("model", modelTransform);
		this->colouredPipeline.setMatrix4x4Uniform("viewProj", viewProj);

		//this->colouredPipeline.setVec4Uniform("LightUBlock.uLights[0].position", glm::vec4{10.0, 1.0, 0.0, 1.0});
		//this->colouredPipeline.setVec4Uniform("LightUBlock.uLights[0].color", glm::vec4{ 1.0, 1.0, 1.0, 0.0 });
		this->colouredPipeline.setVec4Uniform("eyePosition", glm::vec4{ camera->Position, 1.0 });
	} else if ((mesh->flags & ModelFlags::Textured) != 0) {
		this->texturedPipeline.use();
		this->texturedPipeline.setMatrix4x4Uniform("model", modelTransform);
		this->texturedPipeline.setMatrix4x4Uniform("viewProj", viewProj);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);

		//this->texturedPipeline.setVec4Uniform("uLights[0].position", glm::vec4{ 10.0, 1.0, 0.0, 1.0 });
		//this->texturedPipeline.setVec4Uniform("uLights[0].color", glm::vec4{ 1.0, 1.0, 1.0, 0.0 });
		this->texturedPipeline.setVec4Uniform("eyePosition", glm::vec4{ camera->Position, 1.0 });
		//checkError();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		std::cerr << "Model has no pipeline set. Most likely caused by incorrect ModelFlags" << "\n";
	}

	glBindVertexArray(mesh->VAO);

	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, NULL);
}

void RenderSystem::load_model(ModelComponent* modelComponent) {
	for (auto &mesh : modelComponent->meshes) {
		this->load_mesh(&mesh);
	}
}

void RenderSystem::load_mesh(MeshComponent* modelComponent) {
	glGenBuffers(1, &modelComponent->VertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, modelComponent->VertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 3 * modelComponent->vertices.size() * sizeof(GLfloat), modelComponent->vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &modelComponent->IndexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelComponent->IndexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelComponent->indices.size() * sizeof(GLuint), modelComponent->indices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &modelComponent->VAO);
	glBindVertexArray(modelComponent->VAO);

	if ((modelComponent->flags & ModelFlags::Coloured) != 0) {
		glEnableVertexAttribArray(this->colouredPipeline.getVertexAttribIndex("position"));
		glEnableVertexAttribArray(this->colouredPipeline.getVertexAttribIndex("normal"));

		glGenBuffers(1, &modelComponent->ColorBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->ColorBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 4 * modelComponent->colors.size() * sizeof(GLfloat), modelComponent->colors.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(this->colouredPipeline.getVertexAttribIndex("color"), 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

		glGenBuffers(1, &modelComponent->NormalBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->NormalBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 3 * modelComponent->normals.size() * sizeof(GLfloat), modelComponent->normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(this->colouredPipeline.getVertexAttribIndex("normal"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->VertexBufferObject);
		glVertexAttribPointer(this->colouredPipeline.getVertexAttribIndex("position"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelComponent->IndexBufferObject);
	} else if ((modelComponent->flags & ModelFlags::Textured) != 0) {
		glEnableVertexAttribArray(this->texturedPipeline.getVertexAttribIndex("position"));
		glEnableVertexAttribArray(this->texturedPipeline.getVertexAttribIndex("texture_pos"));
		glEnableVertexAttribArray(this->texturedPipeline.getVertexAttribIndex("normal"));

		glGenBuffers(1, &modelComponent->TextureCoordBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->TextureCoordBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 2 * modelComponent->texcoords0.size() * sizeof(GLfloat), modelComponent->texcoords0.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(this->texturedPipeline.getVertexAttribIndex("texture_pos"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

		glGenBuffers(1, &modelComponent->NormalBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->NormalBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 3 * modelComponent->normals.size() * sizeof(GLfloat), modelComponent->normals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(this->texturedPipeline.getVertexAttribIndex("normal"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

		this->lightingSystem.addToVAO();

		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->VertexBufferObject);
		glVertexAttribPointer(this->texturedPipeline.getVertexAttribIndex("position"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelComponent->IndexBufferObject);
	} else {
		std::cerr << "ERROR: Either the Textured or Coloured flag must be set in order to load a model" << "\n";
	}
}

void RenderSystem::load_texture(MeshComponent* model, LoadTextureInfo* loadTextureInfo) {
	int width, height, channels;
	unsigned char* data = stbi_load(loadTextureInfo->texture_file, &width, &height, &channels, 0);

	if (stbi_failure_reason()) {
		std::cerr << "ERROR: Failed to load image " << loadTextureInfo->texture_file << " : " << stbi_failure_reason() << "\n";
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	model->textureID = texture;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, loadTextureInfo->wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, loadTextureInfo->wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, loadTextureInfo->minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, loadTextureInfo->magFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, loadTextureInfo->type, width, height, 0, loadTextureInfo->type, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

void RenderSystem::checkError() {
	switch (glGetError()) {
	case GL_NO_ERROR:
		std::cout << "No errors\n";
		break;
	case GL_INVALID_ENUM:
		std::cout << "Invalid Enum\n";
		break;
	case GL_INVALID_VALUE:
		std::cout << "Invalid Value\n";
		break;
	case GL_INVALID_OPERATION:
		std::cout << "Invalid Operation\n";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		std::cout << "Invalid Framebuffer Operaiton\n";
		break;
	case GL_OUT_OF_MEMORY:
		std::cout << "GL Out of Memory\n";
		break;
	}
}

void RenderSystem::add_model(EntityID id, uint64_t entityFlags) {
	this->entities.push_back(id);

	if ((entityFlags & EntityCreateFlags::Light) != 0) {
		this->lightingSystem.addEntity(id);
	}
}
