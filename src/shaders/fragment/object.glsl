#version 450 core
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

out vec4 FragColor;

in vec3 verticiesCords;
in vec3 normalCords;
in vec2 textureCords;

uniform sampler2D texture1;

uniform vec3 objectColor;
uniform vec3 lightColor;

in vec3 fragPos;
uniform vec3 lightPos;
uniform vec3 camPos;

void main() {
	vec3 ambient = lightColor * material.ambient;

	vec3 normalized = normalize(normalCords);
	vec3 lightDir = normalize(lightPos - fragPos);
	float angleDif = max(dot(lightDir, normalized), 0.0);
	vec3 diffuse = lightColor * (angleDif * material.diffuse);

	vec3 viewDir = normalize(camPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normalized);
	float angleSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = lightColor * (angleSpec * material.specular);

	vec3 light = ambient + diffuse + specular;
	FragColor = vec4(light * vec3(texture(texture1, textureCords)), 1.0);
}
