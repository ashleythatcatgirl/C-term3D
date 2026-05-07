#version 450 core

out vec4 FragColor;

in vec3 vVCoords;
in vec3 vNCoords;
in vec2 vTCoords;
in vec3 vFPos;

#define LIGHTS 2

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;

	uint shininess;
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

uniform vec3 camPos;
uniform bool selected;

const float minFog = 96.0;
const float maxFog = 128.0;

vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDir, vec3 fPos, vec3 difSample, vec3 specSample);

void main() {
	vec3 normal = normalize(vNCoords);
	vec3 viewDir = normalize(camPos - vFPos);

	vec3 result;

	vec3 difSample = vec3(texture(material.texture_diffuse1, vTCoords));
	vec3 specSample = vec3(texture(material.texture_specular1, vTCoords));

	for (int l = 0; l < LIGHTS; l++) {
		result += CalculatePointLight(light[l], normal, viewDir, vFPos, difSample, specSample);
	}

	float dist = length(vFPos - camPos);
	float fogFactor = (maxFog - dist) / (maxFog - minFog);
	float alpha = clamp(fogFactor, 0.0, 1.0);

	if (selected == true) {
		result = (vec3(1.0, 1.0, 1.0) - result);
		clamp(result, 0.0, 1.0);
	}
	
	FragColor = vec4(vec3(result), alpha);
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDir, vec3 fPos, vec3 difSample, vec3 specSample) {
	float distance = length(light.position - fPos);
	float attenuation = 1.0 / (1.0 + light.attLinear * distance + light.attQuadratic * pow(distance, 2));

	vec3 lightDir = normalize(light.position - fPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float angleDif = max(dot(lightDir, normal), 0.0);
	float angleSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * difSample;
	vec3 diffuse = light.diffuse * difSample * angleDif;
	vec3 specular = light.specular * specSample * angleSpec;

	vec3 result = attenuation * (ambient + diffuse + specular);

	return result;
}
