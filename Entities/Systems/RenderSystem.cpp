#include "RenderSystem.h"
#include "RenderSystem.h"
#include "RenderSystem.h"
#include "RenderSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "stb_image.h"

void RenderSystem::init() {
	this->colouredPipeline.init("Resources/shaders/coloured.vert", "Resources/shaders/coloured.frag");
	this->texturedPipeline.init("Resources/shaders/textured.vert", "Resources/shaders/textured.frag");

	stbi_set_flip_vertically_on_load(true);
}

void RenderSystem::render(std::vector<ModelComponent>* modelComponents, std::vector<PositionComponent>* positionComponents, Camera* camera, int width, int height) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	auto look_vector = camera->Position + camera->LookAt;

	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	view = glm::lookAt(camera->Position, look_vector, camera->Up);

	for (EntityID id : this->entities) {
		glm::mat4 modelTransform = glm::mat4(1.0f);
		modelTransform = glm::translate(modelTransform, positionComponents->at(id));

		for (int i = 0; i < modelComponents->at(id).meshes.size(); i++) {
			this->renderMesh(modelTransform, projection, view, &modelComponents->at(id).meshes[i]);
		}
	}
}

void RenderSystem::renderMesh(glm::mat4 modelTransform, glm::mat4 projection, glm::mat4 view, MeshComponent* mesh) {
	if ((mesh->flags & ModelFlags::Coloured) != 0) {
		this->colouredPipeline.use();
		this->colouredPipeline.setMatrix4x4Uniform("model", modelTransform);
		this->colouredPipeline.setMatrix4x4Uniform("projection", projection);
		this->colouredPipeline.setMatrix4x4Uniform("view", view);
	} else if ((mesh->flags & ModelFlags::Textured) != 0) {
		this->texturedPipeline.use();
		this->texturedPipeline.setMatrix4x4Uniform("model", modelTransform);
		this->texturedPipeline.setMatrix4x4Uniform("projection", projection);
		this->texturedPipeline.setMatrix4x4Uniform("view", view);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		std::cerr << "Model has no pipeline set. Most likely caused by incorrect ModelFlags" << "\n";
	}

	glBindVertexArray(mesh->VAO);

	glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, NULL);
}

void RenderSystem::load_model(ModelComponent* modelComponent) {
	for (auto& mesh : modelComponent->meshes) {
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
		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->VertexBufferObject);
		glVertexAttribPointer(this->colouredPipeline.getVertexAttribIndex("position"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelComponent->IndexBufferObject);
		glEnableVertexAttribArray(this->colouredPipeline.getVertexAttribIndex("position"));
	} else if ((modelComponent->flags & ModelFlags::Textured) != 0) {
		glGenBuffers(1, &modelComponent->TextureCoordBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->TextureCoordBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 2 * modelComponent->texcoords0.size() * sizeof(GLfloat), modelComponent->texcoords0.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(this->texturedPipeline.getVertexAttribIndex("texture_pos"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

		glBindBuffer(GL_ARRAY_BUFFER, modelComponent->VertexBufferObject);
		glVertexAttribPointer(this->texturedPipeline.getVertexAttribIndex("position"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelComponent->IndexBufferObject);
		glEnableVertexAttribArray(this->texturedPipeline.getVertexAttribIndex("position"));
		glEnableVertexAttribArray(this->texturedPipeline.getVertexAttribIndex("texture_pos"));
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

void RenderSystem::add_model(EntityID id) {
	this->entities.push_back(id);
}
