#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma once

class Shader {
public:
	std::fstream src;
	std::stringstream buff;
	std::string S;
	const char* src_string;
	char shaderType;

	// For Shader Compilation
	GLuint shader;

	Shader(const char* source, char type) {
		shaderType = type;

		std::fstream src(source);
		buff << src.rdbuf();
		S = buff.str();
		src_string = S.c_str();

		compileShader();
	}

	void compileShader() {

		// vertex shader
		if (shaderType == 'v') {
			shader = glCreateShader(GL_VERTEX_SHADER);
		}
		else {
			shader = glCreateShader(GL_FRAGMENT_SHADER);
		}

		glShaderSource(shader, 1, &src_string, NULL);
		glCompileShader(shader);
	}

	GLuint getShader() {
		return shader;
	}
};
