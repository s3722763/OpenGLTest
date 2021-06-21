#pragma once
#include <tiny_gltf.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "EntityComponents.h"

#include <vector>

namespace Entity::Loaders {
	std::vector<ModelComponent> load_gltf(const char* file);
	ModelComponent load_obj(const char* file);

	template<typename T>
	void load_gltf_data(size_t accessorIndex, tinygltf::Model* model, std::vector<T>* result) {
		auto& accessor = model->accessors[accessorIndex];
	}
}