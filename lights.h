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
/*
	This class is used to represent the light that emits from the submarine onto the objects 
	in the game world. It is able to control the intensity of its light whenever the user
	presses the 'F' key.
*/
class PointLight : public Light {
public:
	glm::vec3 color_brightness;
	glm::vec3 direction;

	// Class constructor
	PointLight(glm::vec3 position, glm::vec3 color) {
		this->position = position;
		this->color = color; 
		this->color_brightness = glm::vec3(0.1, 0.1, 0.1);
		this->ambientColor = glm::vec3(0.04, 0.08, 0.24);
		this->ambientStr = 0.1f;
	}
	// This method controls the strength of the light emitted from the submarine by pressing the 'F' button
	void setBrightness(int x) {
		switch (x) {
			case 0: specularStr = 0.2f;
					specular = 1.0f;
					color_brightness = glm::vec3(0.1, 0.1, 0.1);
					break;
			case 1: specularStr = 0.4f;
					specular = 6.0f;
					color_brightness = glm::vec3(0.5, 0.5, 0.5);
					break;
			case 2: specularStr = 0.8f;
					specular = 9.0f;
					color_brightness = glm::vec3(0.7, 0.7, 0.7);
					break;
			default:
					specularStr = 0.20f;
					specular = 3.0f;
					color_brightness = glm::vec3(0.1, 0.1, 0.1);
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

/*
	This class is primarily used to cast a light downwards onto the objects from the top.
	The values needed are already inherited from the parent Light class. 
*/

class DirectionLight : public Light {
public:

	DirectionLight(glm::vec3 position, glm::vec3 color) {
		this->position = position;
		this->color = color;
	}
};
