#pragma once
#include <tiny_gltf.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "EntityComponents.h"
#include "../Utilities/Utilities.h"
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Entity::Loaders {
	//std::vector<ModelComponent> load_gltf(const char* file);
	//ModelComponent load_obj(const char* file);
	std::vector<MeshComponent> load_model(const char* file);
	void processNode(aiNode* node, const aiScene* scene, std::vector<MeshComponent>* meshes);
	void processMesh(aiMesh* mesh, const aiScene* scene, std::vector<MeshComponent>* meshes);

	template<typename T>
	void load_gltf_data(size_t accessorIndex, tinygltf::Model* model, std::vector<T>* result) {
		auto& accessor = model->accessors[accessorIndex];
		size_t startOffset = accessor.byteOffset;
		size_t bufferViewIndex = accessor.bufferView;

		auto& bufferView = model->bufferViews[bufferViewIndex];
		size_t bufferViewOffset = bufferView.byteOffset;
		size_t numberOfElements = sizeof(T);

		for (int i = 0; i < accessor.count; i++) {
			size_t offset = startOffset + bufferViewOffset + (i * bufferView.byteStride);
			T data{};

			Utilities::read_buffer_to_type(&model->buffers[bufferView.buffer].data, offset, &data);
			result->push_back(data);
		}
	}
}