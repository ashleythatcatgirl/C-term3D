#version 450 core
out vec4 FragColor;

in vec3 verticiesCords;
in vec3 normalCords;
in vec2 textureCords;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 lightPos;
in vec3 fragPos;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = lightColor * ambientStrength;

	vec3 norm = normalize(normalCords);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//FragColor = vec4(ambient * objectColor, 1.0);
	FragColor = vec4((ambient + diffuse) * vec3(texture(texture1, textureCords)), 1.0);
}
