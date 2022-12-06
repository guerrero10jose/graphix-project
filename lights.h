#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma once

class Light {
public:
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 specular;

	Light() {

	}
};

class DirLight : public Light {

};

class PointLight : public Light {
public:
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

};
