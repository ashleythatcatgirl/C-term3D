#version 450 core
layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

out vec3 vVertexCoords;
out vec3 vNormalCoords;
out vec2 vTextureCoords;
out vec3 vFragmentPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool selected;

const float OUTLINE_WIDTH = 0.1;

void main() {
	vec3 position;
	position = aVertex;

	vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

	position += Normal * (OUTLINE_WIDTH * int(selected));

	gl_Position = projection * view * model * vec4(position, 1.0);
	vFragmentPos = vec3(model * vec4(position, 1.0));

	vVertexCoords = position;
	vNormalCoords = Normal;
	vTextureCoords = aTexture;
}
