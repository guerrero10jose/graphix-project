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
	glm::vec3 position;
	glm::vec3 ambientColor;
	float ambientStr;
	float specular;
	float specularStr;
	glm::vec3 color;
};

class DirLight : public Light {
public:
	glm::vec3 color_brightness;

	glm::vec3 direction;
	DirLight(glm::vec3 position, glm::vec3 color) {
		this->position = position;
		this->color = color; 
		this->color_brightness = glm::vec3(0.1, 0.1, 0.1);
		this->ambientColor = glm::vec3(0.04, 0.08, 0.24);
		this->ambientStr = 0.1f;
	}
	void setBrightness(int x) {
		switch (x) {
			case 0: specularStr = 0.2f;
					specular = 1.0f;
					color_brightness = glm::vec3(0.10, 0.10, 0.10);
					break;
			case 1: specularStr = 0.4f;
					specular = 6.0f;
					color_brightness = glm::vec3(0.5, 0.5, 0.5);
					break;
			case 2: specularStr = 0.8f;
					specular = 9.0f;
					color_brightness = glm::vec3(0.8, 0.8, 0.8);
					break;
			default:
					specularStr = 0.20f;
					specular = 3.0f;
					color_brightness = glm::vec3(0.10, 0.10, 0.10);
		}
	}
	void assignlightDir(glm::vec3 newDir) {
		direction = newDir;
	}
	void updatePosition(glm::vec3 position) {
		this->position = position;
	}
	glm::vec3 getlightDir() {
		return direction;
	}
	glm::vec3 getPosition() {
		return position;
	}
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
