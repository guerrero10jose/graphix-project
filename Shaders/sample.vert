#version 330 core // Version

// Gets the data at Atrrib Index 0
// Converts it and stores it into a Vec3
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 m_tan;
layout(location = 4) in vec3 m_btan;

out vec2 texCoord;
out vec3 normCoord;
out vec3 fragPos;

out mat3 TBN;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

void main() {

	gl_Position = projection * view * transform * vec4(aPos, 1.0);

	texCoord = aTex;

	mat3 modelMat = mat3(
		transpose(inverse(transform))
		);

	// get normal matrix and apply to vertex normal
	normCoord = modelMat * vertexNormal;

	vec3 T = normalize(modelMat * m_tan);
	vec3 B = normalize(modelMat * m_btan);
	vec3 N = normalize(normCoord);

	TBN = mat3(T, B, N);

	// get vertex position in world space
	fragPos = vec3(transform * vec4(aPos, 1.0));

}