#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#pragma once

class Camera {
public:
	glm::mat4 projection;
	int currCam;
	float width, height, x_cam, y_cam, z_cam;

	// Other things
	glm::vec3 cameraPos, WorldUp, Center;
	glm::mat4 cameraPositionMatrix, cameraOrientation;
	glm::vec3 F, R, U;
	glm::mat4 viewMatrix;
	glm::vec3 cameraPos2;

	glm::vec3 currCenter;

	// Starting Projection (Perspective)
	Camera(float window_width, float window_height, float x, float y, float z) {

		projection = glm::perspective(
			glm::radians(60.0f),
			window_height / window_width,
			0.1f,
			100.0f
		);

		width = window_width;
		height = window_height;

		x_cam = x;
		y_cam = y;
		z_cam = z;

		cameraPos = glm::vec3(x_cam, y_cam, z_cam);
		cameraPos2 = glm::vec3(0, 4.f, 20.f);

		// 0 for first person 1 for third person
		currCam = 0;
	}

	int getCurrentCam() {
		return currCam;
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
			projection = glm::perspective(
				glm::radians(45.0f),
				height / width,
				0.1f,
				100.0f
			);
			// TODO: Fix This
			/*
			projection = glm::ortho(-0.5f,
				0.5f,
				-0.5f,
				0.5f,
				-100.f,
				100.f
			); */

			currCam = 1;
		}
	}

	glm::mat4 getProjection() {
		return projection;
	}

	void project(glm::vec3 cent) {

		cameraPositionMatrix = glm::translate(glm::mat4(1.0f),
			cameraPos * -1.0f);

		WorldUp = glm::vec3(0, 1.0f, 0);

		switch (currCam) {
		case 0:
			currCenter = cent;
				//glm::vec3(0, 0.0f, -1.0f);
			break;
		case 1:
			currCenter = cent;
			break;
		}

		// forward
		F = glm::vec3(currCenter - cameraPos);
		F = glm::normalize(F);

		// right
		R = glm::cross(F, WorldUp);

		// up
		U = glm::cross(R, F);

		// orientation matrix
		cameraOrientation = glm::mat4(1.0f);

		cameraOrientation[0][0] = R.x;
		cameraOrientation[1][0] = R.y;
		cameraOrientation[2][0] = R.z;

		cameraOrientation[0][1] = U.x;
		cameraOrientation[1][1] = U.y;
		cameraOrientation[2][1] = U.z;

		cameraOrientation[0][2] = -F.x;
		cameraOrientation[1][2] = -F.y;
		cameraOrientation[2][2] = -F.z;

		switch (currCam) {
		case 0:
			viewMatrix = glm::lookAt(cameraPos, cameraPos + currCenter, WorldUp);
			break;
		case 1:
			viewMatrix = glm::lookAt(cameraPos2, cameraPos2 + currCenter, WorldUp);
			break;
		}
	}

	glm::vec3 getCameraPos() {
		return cameraPos;
	}

	glm::vec3 getCameraPos2() {
		return cameraPos2;
	}

	glm::mat4 getViewMatrix() {
		return viewMatrix;
	}

	glm::vec3 getCurrCenter() {
		return currCenter;
	}

	void updateCameraPos(float camSpeed, char c) {

		switch (c) {
			// forward / backward
		case 'f':
			cameraPos += camSpeed * currCenter;
			cameraPos2 += camSpeed * currCenter;
			break;
		case 'b':
			cameraPos -= camSpeed * currCenter;
			cameraPos2 -= camSpeed * currCenter;
			break;
			// ascend / descend
		case 'q':
			cameraPos += glm::normalize(glm::cross(R, currCenter)) * camSpeed;
			cameraPos2 += glm::normalize(glm::cross(R, currCenter)) * camSpeed;
			break;
		case 'e':
			cameraPos -= glm::normalize(glm::cross(R, currCenter)) * camSpeed;
			cameraPos2 -= glm::normalize(glm::cross(R, currCenter)) * camSpeed;
			break;
		// rotate left / right
		case 'a':
			cameraPos.x += camSpeed;
			cameraPos.y += camSpeed;
			break;

		}

	}

};