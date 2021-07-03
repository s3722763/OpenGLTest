#include <SDL.h>

#include <glad/gl.h>

#include "Entities/Models.h"
#include <glm/vec3.hpp>

#include "Entities/EntityComponents.h"
#include "Utilities/Pipeline.h"
#include "Systems/RenderSystem.h"
#include "Entities.h"
#include <Systems/MovementSystem.h>

class Engine {
private:
	SDL_Window* window;
	SDL_GLContext context;

	RenderSystem renderSystem;
	MovementSystem movementSystem;

	EntityManager entityManager;

	GLuint VBO = 0;
	GLuint IBO = 0;
	GLuint VAO = 0;

	bool init_GL();
	void initImGui();
	void showEntityCreator();
	void update(float delta_s);
	void draw(float delta_s);
	void save();
	void create_box(glm::vec3 position, glm::vec3 velocity);
	void create_fire_pit();
	void create_tree();
	void create_light();

	int width = 1280;
	int height = 720;

public:
	//glm::vec3 camera_position{ 0, 0, 5 };
	//glm::vec3 camera_look_direction{ 0, 0, -1 };
	//glm::vec3 camera_up{ 0, 1, 0 };
	Camera camera {
		.Position{0, 0, 5},
		.LookAt{0, 0, -1},
		.Up{0, 1, 0}
	};
	float camera_yaw = -90;
	float camera_pitch = 0;

	void init();
	void load_media();
	void run();

	void deinit();
};