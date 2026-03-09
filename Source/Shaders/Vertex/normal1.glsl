#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

out vec3 verticiesCords;
out vec3 normalCords;
out vec2 textureCords;

out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	fragPos = vec3(model * vec4(aPos, 1.0));

	vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

	verticiesCords = aPos;
	normalCords = Normal;
	textureCords = aTexture;
}
