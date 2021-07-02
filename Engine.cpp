#include "Engine.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Logger.hpp>

const char* glsl_version = "#version 330 core";

void Engine::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Error initializating SDL: " << SDL_GetError() << "\n";
	} else {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		this->window = SDL_CreateWindow("OpenGL Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

		if (this->window == nullptr) {
			std::cout << "Could not create SDL window: " << SDL_GetError() << "\n";
			SDL_Quit();
		} else {
			this->context = SDL_GL_CreateContext(this->window);
			SDL_GL_MakeCurrent(this->window, this->context);
			if (this->context == nullptr) {
				std::cout << "Could not create OpenGL context: " << SDL_GetError() << "\n";

				SDL_DestroyWindow(this->window);
				SDL_Quit();
			} else {
				bool err = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0;

				if (err == true) {
					std::cout << "Failed to initalize OpenGL loader" << "\n";
				}
				// Set V-Sync
				if (SDL_GL_SetSwapInterval(1) < 0) {
					std::cout << "Unable to enable V-Sync: " << SDL_GetError() << "\n";
				}

				//SDL_ShowCursor(SDL_DISABLE);
				SDL_SetRelativeMouseMode(SDL_TRUE);

				// Initialise OpenGL
				if (!this->init_GL()) {
					std::cout << "Unable to initialise OpenGL";
					SDL_GL_DeleteContext(this->context);
					SDL_DestroyWindow(this->window);
					SDL_Quit();
				}

				Assimp::DefaultLogger::create("AssimpLog.txt", Assimp::Logger::VERBOSE);
			}
		}
	}

	this->renderSystem.init();
	
	this->initImGui();
}

void Engine::run() {
	SDL_Event e;
	bool quit = false;

	while (!quit) {
		std::chrono::steady_clock::time_point static time = std::chrono::steady_clock::now();
		auto current_time = std::chrono::steady_clock::now();

		float duration_seconds = std::chrono::duration<float>(current_time - time).count();
		time = current_time;

		while (SDL_PollEvent(&e) != 0) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_MOUSEMOTION) {
				// Only move the camera when the window has eaten the mouse
				if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
					const float sensitivity = 0.02f;

					this->camera_yaw += sensitivity * e.motion.xrel;
					this->camera_pitch -= sensitivity * e.motion.yrel;
					//std::cout << e.motion.xrel << " - " << e.motion.yrel << "\n";
					//auto rotation_matrix = glm::eulerAngleXY(glm::radians(this->camera_pitch), glm::radians(this->camera_yaw));

					auto new_look_direction = glm::vec3(std::cosf(glm::radians(this->camera_yaw)) * std::cosf(glm::radians(this->camera_pitch)),
						std::sinf(glm::radians(this->camera_pitch)),
						std::sin(glm::radians(this->camera_yaw)) * std::cos(glm::radians(this->camera_pitch)));

					this->camera.LookAt = glm::normalize(new_look_direction);
				}
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					quit = true;
				} else if (e.key.keysym.scancode == SDL_SCANCODE_LCTRL) {
					uint32_t shown = SDL_GetRelativeMouseMode();

					std::cout << shown << "\n";
					if (shown == SDL_TRUE) {
						//SDL_ShowCursor(SDL_DISABLE);
						SDL_SetRelativeMouseMode(SDL_FALSE);
					} else {
						SDL_SetRelativeMouseMode(SDL_TRUE);
						//SDL_ShowCursor(SDL_ENABLE);
					}
				}
			} else if (e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
					glViewport(0, 0, e.window.data1, e.window.data2);
					this->width = e.window.data1;
					this->height = e.window.data2;
				}
			}
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(this->window);
		ImGui::NewFrame();

		this->update(duration_seconds);
		this->draw(duration_seconds);

		//bool showDemo = true;
		//ImGui::ShowDemoWindow(&showDemo);

		this->showEntityCreator();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(this->window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

bool Engine::init_GL() {
	bool success = true;

	glViewport(0, 0, this->width, this->height);

	return success;
}

void Engine::initImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window, this->context);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void Engine::showEntityCreator() {
	static float positionInput[3] = { 0, 0, 0 };
	static float velocityInput[3] = { 0, 0, 0 };

	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
	ImGui::Begin("Entity Creator");
	ImGui::BeginTable("EntityProperties", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);

	ImGui::PushID("PositionInput");
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();
	ImGui::InputFloat3("Position", positionInput);
	ImGui::PopID();

	ImGui::PushID("VelocityInput");
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();
	ImGui::InputFloat3("Velocity", velocityInput);
	ImGui::PopID();

	ImGui::PushID("CreateEntityButton");
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();

	if (ImGui::Button("Create Entity")) {
		this->create_box(glm::vec3{ positionInput[0], positionInput[1], positionInput[2] }, { velocityInput[0], velocityInput[1], velocityInput[2] });
	}

	ImGui::PopID();

	ImGui::EndTable();
	ImGui::End();
}

void Engine::update(float delta_s) {
	const uint8_t* currentKeyStates = SDL_GetKeyboardState(NULL);

	float speed = 5 * delta_s;

	if (currentKeyStates[SDL_SCANCODE_W] == 1) {
		this->camera.Position += speed * this->camera.LookAt;
	} else if (currentKeyStates[SDL_SCANCODE_S] == 1) {
		this->camera.Position -= speed * this->camera.LookAt;
	}

	if (currentKeyStates[SDL_SCANCODE_A] == 1) {
		this->camera.Position -= glm::normalize(glm::cross(this->camera.LookAt, this->camera.Up)) * speed;
	} else if (currentKeyStates[SDL_SCANCODE_D] == 1) {
		this->camera.Position += glm::normalize(glm::cross(this->camera.LookAt, this->camera.Up)) * speed;
	}

	this->movementSystem.update(this->entityManager.getPositionComponents(), this->entityManager.getMovementComponents(), delta_s);
}

void Engine::load_media() {
	GLfloat vertex_data[] = {
						-0.5f, -0.5f,
						0.5f, -0.5f,
						0.5f, 0.5f,
						-0.5f, 0.5f
	};

	// IBO data
	GLuint index_data[] = { 0, 1, 2, 3 };

	/*glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);

	glGenBuffers(1, &this->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), index_data, GL_STATIC_DRAW);*/

	//auto model = Entity::Loaders::load_gltf("Resources/gltf/triangles.json").at(0);
	//this->create_box({ 0, -1, 0 }, { 0, 0, 0 });

	//this->create_fire_pit();
	this->create_tree();
}

void Engine::draw(float delta_s) {
	this->renderSystem.render(this->entityManager.getModelComponents(), this->entityManager.getPositionComponents(), &this->camera, this->width, this->height);
}

void Engine::save() {
	nlohmann::json j;

	j["version"] = 1;
	this->entityManager.saveEntities(&j);

	std::ofstream file("save.json");
	file << std::setw(4) << j << std::endl;
}

void Engine::create_box(glm::vec3 position, glm::vec3 velocity) {
	auto meshes = Entity::Loaders::load_model("Resources/obj/cube.obj");

	ModelComponent modelComponent{};
	modelComponent.meshes = meshes;

	this->renderSystem.load_model(&modelComponent);

	MovementComponent movement{};
	movement.velocity = velocity;

	EntityCreateInfo newEntity{};
	newEntity.name = "Box";
	newEntity.modelComponent = modelComponent;
	newEntity.positionComponent = position;
	newEntity.movementComponent = movement;

	EntityID id = this->entityManager.addEntity(&newEntity);
	this->renderSystem.add_model(id);
	this->movementSystem.add_entity(id);
}

void Engine::create_fire_pit() {
	auto meshes = Entity::Loaders::load_model("Resources/models/firepit/untitled.gltf");
	meshes[0].flags |= ModelFlags::Textured;

	LoadTextureInfo loadTextureInfo{};
	loadTextureInfo.texture_file = "Resources/models/firepit/trn_Firepit_AlbedoTransparency.png";
	loadTextureInfo.type = GL_RGB;
	loadTextureInfo.flags = LoadTextureInfoFlags::Texture;
	this->renderSystem.load_texture(&meshes[0], &loadTextureInfo);

	ModelComponent modelComponent{};
	modelComponent.meshes = meshes;

	this->renderSystem.load_model(&modelComponent);

	PositionComponent lampPosition = { 0, 0, 0 };

	EntityCreateInfo newEntity{};
	newEntity.name = "firepit";
	newEntity.modelComponent = modelComponent;
	newEntity.positionComponent = lampPosition;

	EntityID id = this->entityManager.addEntity(&newEntity);
	this->renderSystem.add_model(id);
}

void Engine::create_tree() {
	auto meshes = Entity::Loaders::load_model("Resources/models/trees/Forest.glb");
	LoadTextureInfo loadTextureInfo{};
	loadTextureInfo.texture_file = "Resources/models/trees/PUSHILIN_forest.png";
	loadTextureInfo.type = GL_RGBA;
	loadTextureInfo.flags = LoadTextureInfoFlags::Texture;
	this->renderSystem.load_texture(&meshes[0], &loadTextureInfo);

	ModelComponent modelComponent{};
	modelComponent.meshes = meshes;

	this->renderSystem.load_model(&modelComponent);

	PositionComponent treePosition = { 3, 0, 0 };

	EntityCreateInfo newEntity{};
	newEntity.name = "bigtree";
	newEntity.modelComponent = modelComponent;
	newEntity.positionComponent = treePosition;

	EntityID id = this->entityManager.addEntity(&newEntity);
	this->renderSystem.add_model(id);
}

void Engine::deinit() {
	this->save();
	SDL_DestroyWindow(this->window);
	this->window = nullptr;

	SDL_Quit();
}