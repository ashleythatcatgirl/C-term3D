#version 450 core
out vec4 FragColor;

in vec3 verticiesCords;
in vec3 normalCords;
in vec2 textureCords;

struct Material {
	vec3 specular;
	float shininess;

	sampler2D textureDiffuse;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;


in vec3 fragPos;
uniform vec3 lightPos;
uniform vec3 camPos;

void main() {
	vec3 ambient = light.ambient * vec3(texture(material.textureDiffuse, textureCords));

	vec3 normalized = normalize(normalCords);
	vec3 lightDir = normalize(lightPos - fragPos);
	float angleDif = max(dot(lightDir, normalized), 0.0);
	vec3 diffuse = light.diffuse * angleDif * vec3(texture(material.textureDiffuse, textureCords));

	vec3 viewDir = normalize(camPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normalized);
	float angleSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * angleSpec * material.specular;

	vec3 light = ambient + diffuse + specular;
	FragColor = vec4(light, 1.0);
}
