// OpenGLTest.cpp : Defines the entry point for the application.
//

#include "OpenGLTest.h"
#include "Engine.h"
#undef main

int main(int argc, char** argv)
{
	Engine engine{};
	engine.init();
	engine.load_media();
	engine.run();
	engine.deinit();
	return 0;
}
