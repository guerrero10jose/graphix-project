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
	glm::mat4 projection, savedProj;
	int currCam, currPersp;
	float width, height, x_cam, y_cam, z_cam;

	// Other things
	glm::vec3 cameraPos, WorldUp, Center;
	glm::mat4 cameraPositionMatrix, cameraOrientation;
	glm::vec3 F, R, U;
	glm::mat4 viewMatrix;
	glm::vec3 cameraPos2, cameraPos3;

	glm::vec3 currCenter;
	glm::vec3 orthovec;

	// Starting Projection (Perspective)
	Camera(float window_width, float window_height, float x, float y, float z) {

		projection = glm::perspective(
			glm::radians(60.0f),
			window_height / window_width,
			0.1f,
			100.0f
		);

		savedProj = projection;

		width = window_width;
		height = window_height;

		x_cam = x;
		y_cam = y;
		z_cam = z;

		cameraPos = glm::vec3(x_cam, y_cam, z_cam);
		cameraPos2 = glm::vec3(0, y_cam, 20.f);
		cameraPos3 = glm::vec3(x_cam, y_cam, z_cam);
		orthovec = glm::vec3(0.f, 5.f, 0.f);

		// 0 for first person 1 for third person
		currCam = 0;
		// 0 for persp 1 for ortho
		currPersp = 0;
	}

	int getCurrentCam() {
		return currCam;
	}

	void changeCam() {
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

			currCam = 1;
		}

		savedProj = projection;
	}

	void changePerspective() {

		// if ortho change to persp
		if (currPersp) {

			projection = savedProj;
			currPersp = 0;
		}
		// change to ortho
		else {
			float aspect = float(width / height);
			// ortho(-(800.0f / 2.0f), 800.0f / 2.0f, 600.0f / 2.0f, -(600.0f / 2.0f), -1000.0f, 1000.0f);
			// glm::ortho(-aspect, aspect, -1.0f, 1.0f, zNear, zFar);
			projection = glm::ortho(-(width / 20),
				width / 20,
				-(height / 20),
				height / 20,
				-1000.f,
				1000.f
			);

			currPersp = 1;
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

		if (currPersp == 0) {
			switch (currCam) {

			case 0:
				viewMatrix = glm::lookAt(cameraPos, cameraPos + currCenter, WorldUp);
				break;
			case 1:
				viewMatrix = glm::lookAt(cameraPos2, cameraPos2 + currCenter, WorldUp);
				break;

			}
		}
		else {
			viewMatrix = glm::lookAt(cameraPos3, cameraPos3 + orthovec, glm::vec3(0, 0, -1.0f));
		}

	}

	glm::vec3 getCameraPos() {
		return cameraPos;
	}

	glm::vec3 getCameraPos2() {
		return cameraPos2;
	}

	/*
		Returns the view matrix of the camera
	*/
	glm::mat4 getViewMatrix() {
		return viewMatrix;
	}

	/* 
		Returns the Current Center of the Ship / View
	*/
	glm::vec3 getCurrCenter() {
		return currCenter;
	}

	/*
		Based on hotkeys, updates camerapos accordingly,
		for both first person and third person view
	*/
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
			cameraPos -= glm::normalize(glm::cross(R, currCenter)) * camSpeed;
			cameraPos2 -= glm::normalize(glm::cross(R, currCenter)) * camSpeed;
			break;
		case 'e':
			cameraPos += glm::normalize(glm::cross(R, currCenter)) * camSpeed;
			cameraPos2 += glm::normalize(glm::cross(R, currCenter)) * camSpeed;

			if (int(cameraPos2.y) >= 0 || int(cameraPos.y) >= 0) {
				cameraPos2.y = 0.f;
				cameraPos.y = 0.f;
			}

			break;
		}
	}

	/*
		Function that rotates camera upon key press, used for
		first person view camera
	*/
	glm::vec3 rotateCamera(float camSpeed, float rot_x) 
	{
		float yaw = -90.0f;
		float pitch = 0.f;
		float fov = 90.0f;

		float xoffset = rot_x + cameraPos.x;

		xoffset *= camSpeed;

		yaw += xoffset;

		//prevents the pitch from going out of bounds (prevents backflipping)
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;

		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		return glm::normalize(front);
	}

	/*
		Returns the Current Perspective (Ortho/Perspective) of the
		Camera
	*/
	int getCurrPersp() {
		return currPersp;
	}

	/*
		Returns the Current Depth of the Ship
	*/
	float getDepth() {
		if (currPersp == 0) {
			if (currCam == 0)
				return cameraPos.y;
			else
				return cameraPos2.y;
		}
	}

	/* 
		Pan camera during orthographic view 
		WS - Move camera up and down
		AD - Rotate around the ship axis to see depth
	*/
	void movOrtho(char c, float cameraSpeed) {
		switch (c) {
		case 'w':
			cameraPos3.z -= cameraSpeed;
			//cameraPos3.x += cameraSpeed;
			break;
		case 's':
			cameraPos3.z += cameraSpeed;
			//cameraPos3.y += cameraSpeed; //down movement
			break;
		case 'a':
			orthovec.x -= cameraSpeed;
			//cameraPos3.y += cameraSpeed; 
			break;
		case 'd':
			orthovec.x += cameraSpeed;
			break;
		}
	}

};