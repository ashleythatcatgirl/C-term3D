#version 450 core
out vec4 FragColor;

in vec3 verticiesCords;
in vec3 normalCords;
in vec2 textureCords;

uniform vec3 lightColor;

uniform vec3 lightPos;
uniform vec3 camPos;

void main() {
	FragColor = vec4(lightColor, 1.0);
}
