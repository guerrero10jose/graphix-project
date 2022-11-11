#version 330 core // Version

// Gets the data at Atrrib Index 0
// Converts it and stores it into a Vec3
layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 transform;

void main() {

	gl_Position = projection * transform * vec4(aPos, 1.0);
}