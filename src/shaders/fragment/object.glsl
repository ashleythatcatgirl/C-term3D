#version 450 core
out vec4 FragColor;

in vec3 verticiesCords;
in vec3 normalCords;
in vec2 textureCords;

struct Material {
	sampler2D diffuse;
	sampler2D specular;

	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
};

uniform Material material;
uniform Light light;

in vec3 fragPos;
uniform vec3 camPos;

void main() {
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * pow(distance, 2));

	vec3 ambient = attenuation * light.ambient * vec3(texture(material.diffuse, textureCords));

	vec3 normalized = normalize(normalCords);
	vec3 lightDir = normalize(light.position - fragPos);
	float angleDif = max(dot(lightDir, normalized), 0.0);
	vec3 diffuse = attenuation * light.diffuse * vec3(texture(material.diffuse, textureCords)) * angleDif;

	vec3 viewDir = normalize(camPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normalized);
	float angleSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = attenuation * light.specular * vec3(texture(material.specular, textureCords)) * angleSpec;

	FragColor = vec4(vec3(ambient + diffuse + specular), 1.0);
}
