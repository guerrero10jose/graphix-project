#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma once

class Camera {
public:
	glm::mat4 projection;
	int currCam;
	float width, height;

	// Starting Projection (Perspective)
	Camera(float window_width, 
		float window_height) {
		projection = glm::perspective(
			glm::radians(60.0f),
			window_height / window_width,
			0.1f,
			100.0f
		);

		width = window_width;
		height = window_height;
		
		// 0 for persp 1 for ortho
		currCam = 0;
	}

	void changePersp() {
		if (currCam) {
			projection = glm::perspective(
				glm::radians(60.0f),
				height / width,
				0.1f,
				100.0f
			);

			currCam = 0;
		}
		else {
			// TODO: Fix This
			projection = glm::ortho(-5.f,
				5.f,
				-5.f,
				5.f,
				-100.f,
				100.f);

			currCam = 1;
		}
	}

	glm::mat4 getProjection() {
		return projection;
	}
};