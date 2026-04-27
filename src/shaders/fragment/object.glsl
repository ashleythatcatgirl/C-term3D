#version 450 core
out vec4 FragColor;

in vec3 verticiesCords;
in vec3 normalCords;
in vec2 textureCords;

#define LIGHTS 3

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;

	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;

	float attLinear;
	float attQuadratic;
};

uniform Material material;
uniform Light light[LIGHTS];

in vec3 fragPos;
uniform vec3 camPos;

vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos);

void main() {
	vec3 normal = normalize(normalCords);
	vec3 viewDir = normalize(camPos - fragPos);

	vec3 result;

	for (int l = 0; l < LIGHTS; l++) {
		result += CalculatePointLight(light[l], normal, viewDir, fragPos);
	}
	
	FragColor = vec4(vec3(result), 1.0);
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos) {
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (1.0 + light.attLinear * distance + light.attQuadratic * pow(distance, 2));

	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float angleDif = max(dot(lightDir, normal), 0.0);
	float angleSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = attenuation * light.ambient * vec3(texture(material.texture_diffuse1, textureCords));
	vec3 diffuse = attenuation * light.diffuse * vec3(texture(material.texture_diffuse1, textureCords)) * angleDif;
	vec3 specular = attenuation * light.specular * vec3(texture(material.texture_specular1, textureCords)) * angleSpec;

	vec3 result = (ambient + diffuse + specular);

	return result;
}
