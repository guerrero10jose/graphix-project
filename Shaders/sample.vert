#version 330 core // Version

// Gets the data at Atrrib Index 0
// Converts it and stores it into a Vec3
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTex;

out vec2 texCoord;
out vec3 normCoord;
out vec3 fragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

void main() {

	gl_Position = projection * view * transform * vec4(aPos, 1.0);

	texCoord = aTex;

	// get normal matrix and apply to vertex normal
	normCoord = mat3(
		transpose(inverse(transform))
		) * vertexNormal;

	// get vertex position in world space
	fragPos = vec3(transform * vec4(aPos, 1.0));

}