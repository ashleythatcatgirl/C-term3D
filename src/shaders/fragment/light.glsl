#version 450 core
out vec4 FragColor;

in vec3 verticiesCords;
in vec3 normalCords;
in vec2 textureCords;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 color;
};

uniform Light light;

void main() {
	FragColor = vec4(light.color, 1.0);
}
