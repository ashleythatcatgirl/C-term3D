#version 450 core
layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

out vec3 vVCoords;
out vec3 vNCoords;
out vec2 vTCoords;
out vec3 vFPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool selected;

const float outlineWidth = 0.1;

void main() {
	vec3 position;
	position = aVertex;

	vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

	if (selected == true) {
		position += Normal * outlineWidth;
	}

	gl_Position = projection * view * model * vec4(position, 1.0);
	vFPos = vec3(model * vec4(position, 1.0));


	vVCoords = position;
	vNCoords = Normal;
	vTCoords = aTexture;
}
