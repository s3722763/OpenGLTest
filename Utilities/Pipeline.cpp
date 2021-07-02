#include "Pipeline.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

void Pipeline::init(const char* vertexPath, const char* fragmentPath) {
	this->programID = glCreateProgram();

	// Create vertex shader
	GLuint vertex_shader = 0;
	if (this->load_shader_module(vertexPath, GL_VERTEX_SHADER, &vertex_shader)) {
		// Attach vertex shader to program
		glAttachShader(this->programID, vertex_shader);

		GLuint fragment_shader = 0;
		if (this->load_shader_module(fragmentPath, GL_FRAGMENT_SHADER, &fragment_shader)) {
			glAttachShader(this->programID, fragment_shader);

			glLinkProgram(this->programID);

			// Check for errors
			GLint program_success = GL_TRUE;

			glGetProgramiv(this->programID, GL_LINK_STATUS, &program_success);

			if (program_success != GL_TRUE) {
				std::cout << "Error linking program: " << this->programID << "\n";
				this->print_program_log(this->programID);
			}
		}
	}
}

void Pipeline::setMatrix4x4Uniform(const char* name, const glm::mat4& matrix) {
	glUniformMatrix4fv(glGetUniformLocation(this->programID, name), 1, GL_FALSE, &matrix[0][0]);
}

void Pipeline::setVec4Uniform(const char* name, const glm::vec4& vector) {
	glUniform4fv(glGetUniformLocation(this->programID, name), 1, glm::value_ptr(vector));
}

void Pipeline::setVec3Uniform(const char* name, const glm::vec3& vector) {
	glUniform3fv(glGetUniformLocation(this->programID, name), 1, glm::value_ptr(vector));
}

void Pipeline::setFloatUniform(const char* name, const GLfloat& value) {
	glUniform1f(glGetUniformLocation(this->programID, name), value);
}

GLuint Pipeline::getVertexAttribIndex(const char* name) {
	return glGetAttribLocation(this->programID, name);
}

void Pipeline::use() {
	glUseProgram(this->programID);
}

void Pipeline::destroy() {
	glDeleteProgram(this->programID);
}

bool Pipeline::load_shader_module(const char* file_path, GLuint shader_type, GLuint* id) {
	std::ifstream file(file_path);

	if (!file.is_open()) {
		return false;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	std::string code_str = buffer.str();
	const char* code_char = code_str.c_str();

	GLuint shader = glCreateShader(shader_type);

	glShaderSource(shader, 1, &code_char, NULL);

	glCompileShader(shader);

	GLint shader_compiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);

	if (shader_compiled != GL_TRUE) {
		std::cout << "Unable to compile shader: " << shader << "\n";
		this->print_shader_log(shader);
		return false;
	}

	*id = shader;
	return true;
}

void Pipeline::print_shader_log(GLuint shader) {
	if (glIsShader(shader)) {
		int info_log_length = 0;
		int max_length = info_log_length;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

		char* info_log = new char[max_length];

		glGetShaderInfoLog(shader, max_length, &info_log_length, info_log);

		if (info_log_length > 0) {
			std::cout << info_log << "\n";
		}

		delete[] info_log;
	}
}

void Pipeline::print_program_log(GLuint program) {
	if (glIsProgram(program)) {
		int info_log_length = 0;
		int max_length = info_log_length;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

		char* info_log = new char[max_length];

		glGetProgramInfoLog(program, max_length, &info_log_length, info_log);

		if (info_log_length > 0) {
			std::cout << info_log << "\n";
		}

		delete[] info_log;
	}
}