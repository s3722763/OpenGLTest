#pragma once

#include <glad/gl.h>

#include <glm/mat4x4.hpp>

class Pipeline {
private:
	GLuint programID;

	bool load_shader_module(const char* file_path, GLuint shader_type, GLuint* id);
	void print_program_log(GLuint program);
	void print_shader_log(GLuint shader);

public:
	void init(const char* vertexPath, const char* fragmentPath);

	void setMatrix4x4Uniform(const char* name, const glm::mat4 &matrix);

	GLuint getVertexAttribIndex(const char* name);

	void use();
	void destroy();
};