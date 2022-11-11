#version 330 core // Version

// Gets the data at Atrrib Index 0
// Converts it and stores it into a Vec3
layout(location = 0) in vec3 aPos;

void main() {
	gl_Position = vec4(aPos, 1.0);
}