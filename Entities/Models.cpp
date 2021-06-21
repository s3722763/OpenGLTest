#include "Models.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef NDEBUG
#define TINYGLTF_NOEXCEPTION
#endif

#include <iostream>
#include <tiny_gltf.h>
#include <tiny_obj_loader.h>
#include <glad/gl.h>
#include "../Utilities/Utilities.h"

tinygltf::TinyGLTF loader;

std::vector<MeshComponent> Entity::Loaders::load_model(const char* file) {
	Assimp::Importer importer;
	const auto* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!scene || ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0) || !scene->mRootNode) {
		std::cout << "ERROR: Loading file " << file << " - " << importer.GetErrorString() << "\n";
		return std::vector<MeshComponent>();
	}

	std::vector<MeshComponent> meshes;
	Entity::Loaders::processNode(scene->mRootNode, scene, &meshes);

	return meshes;
}

void Entity::Loaders::processNode(aiNode* node, const aiScene* scene, std::vector<MeshComponent >* meshes) {
	for (auto i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Entity::Loaders::processMesh(mesh, scene, meshes);
	}

	for (auto i = 0; i < node->mNumChildren; i++) {
		Entity::Loaders::processNode(node->mChildren[i], scene, meshes);
	}
}

void Entity::Loaders::processMesh(aiMesh* mesh, const aiScene* scene, std::vector<MeshComponent>* meshes) {
	MeshComponent engineMesh;
	engineMesh.vertices.resize(mesh->mNumVertices);
	engineMesh.normals.resize(mesh->mNumVertices);
	engineMesh.tangents.resize(mesh->mNumVertices);
	engineMesh.texcoords0.resize(mesh->mNumVertices);
	engineMesh.texcoords1.resize(mesh->mNumVertices);

	for (auto i = 0; i < mesh->mNumVertices; i++) {
		engineMesh.vertices[i] = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		engineMesh.normals[i] = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		engineMesh.texcoords0[i] = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
	}

	for (auto i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (auto j = 0; j < face.mNumIndices; j++) {
			engineMesh.indices.push_back(face.mIndices[j]);
		}
	}

	meshes->push_back(engineMesh);
}


/*std::vector<ModelComponent> Entity::Loaders::load_gltf(const char* file) {
	std::string err, warn;
	tinygltf::Model gltf_model{};
	std::vector<ModelComponent> models;

	bool success = loader.LoadASCIIFromFile(&gltf_model, &err, &warn, file);

	if (!warn.empty()) {
		std::cout << "Warn: " << warn << "\n";
	}

	if (!err.empty()) {
		std::cout << "Err: " << err << "\n";
	}

	// Only support for one scene
	auto &scene = gltf_model.scenes[0];
	auto &rootNode = gltf_model.nodes[scene.nodes[0]];
	assert(rootNode.name.c_str() ==  "RootNode");
	// ASSUME: Matrix is just identity matrix with no need for translation

	for (auto& child_id : rootNode.children) {
		auto& mesh = gltf_model.meshes[child_id];

		for (auto& primative: mesh.primitives) {
			size_t normalAccessorIndex = primative.attributes["NORMAL"];
			size_t positionAccessorIndex = primative.attributes["POSITION"];
			size_t tangentAccessorIndex = primative.attributes["TANGENT"];
			size_t texcoord0AccessorIndex = primative.attributes["TEXCOORD_0"];
			size_t texcoord1AccessorIndex = primative.attributes["TEXCOORD_1"];
			size_t indicesAccessorIndex = primative.indices;
			size_t materialAccessorIndex = primative.material;
			size_t mode = primative.mode;

			std::vector<glm::vec3> normals;
			Entity::Loaders::load_gltf_data(normalAccessorIndex, &gltf_model, &normals);

			std::vector<glm::vec3> positions;
			Entity::Loaders::load_gltf_data(positionAccessorIndex, &gltf_model, &positions);

			std::vector<glm::vec4> tangents;
			Entity::Loaders::load_gltf_data(tangentAccessorIndex, &gltf_model, &tangents);

			std::vector<glm::vec2> texcoords0;
			Entity::Loaders::load_gltf_data(texcoord0AccessorIndex, &gltf_model, &texcoords0);

			std::vector<glm::vec2> texcoords1;
			Entity::Loaders::load_gltf_data(texcoord1AccessorIndex, &gltf_model, &texcoords1);

			std::vector<GLuint> indices;
			Entity::Loaders::load_gltf_data(indicesAccessorIndex, &gltf_model, &indices);

			ModelComponent modelComponent{};
			modelComponent.indices = indices;
			modelComponent.normals = normals;
			modelComponent.tangents = tangents;
			modelComponent.vertices = positions;
			modelComponent.texcoords0 = texcoords0;
			modelComponent.texcoords1 = texcoords1;

			models.push_back(modelComponent);
		}
	}

	return models;
}

ModelComponent Entity::Loaders::load_obj(const char* file) {
	ModelComponent model{};
	
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string error;

	if (tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &error, file)) {
		if (!warn.empty()) {
			std::cout << "Warning loading model " << file << ": " << warn << "\n";
		}

		if (!error.empty()) {
			std::cout << "Error loading model " << file << ": " << error << "\n";
		}

		// For each shape
		for (size_t s = 0; s < shapes.size(); s++) {
			size_t index_offset = 0;

			// For each face on a shape
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

				// For each vertex on face
				for (size_t v = 0; v < fv; v++) {
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					
					tinyobj::real_t vx = attributes.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attributes.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attributes.vertices[3 * size_t(idx.vertex_index) + 2];
					
					model.vertices.push_back({vx, vy, vz});

					if (idx.normal_index >= 0) {
						tinyobj::real_t nx = attributes.normals[3 * size_t(idx.normal_index) + 0];
						tinyobj::real_t ny = attributes.normals[3 * size_t(idx.normal_index) + 1];
						tinyobj::real_t nz = attributes.normals[3 * size_t(idx.normal_index) + 2];

						model.normals.push_back({ nx, ny, nz });
					}

					if (idx.texcoord_index >= 0) {
						tinyobj::real_t tx = attributes.texcoords[2 * size_t(idx.texcoord_index) + 0];
						tinyobj::real_t ty = attributes.texcoords[2 * size_t(idx.texcoord_index) + 1];
						model.texcoords0.push_back({ tx, ty });
					}

					tinyobj::real_t red = attributes.colors[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t green = attributes.colors[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t blue = attributes.colors[3 * size_t(idx.vertex_index) + 2];
					model.colors.push_back({ red, green, blue });

					model.indices.push_back(model.indices.size());
				}

				index_offset += fv;

				// Perface material
				shapes[s].mesh.material_ids[f];
			}
		}
	}
	
	return model;
}*/
