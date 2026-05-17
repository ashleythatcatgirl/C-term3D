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

const float MIN_FOG = 64.0;
const float MAX_FOG = 128.0;

vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDir, vec3 difSample, vec3 specSample);
float AddFog();

void main() {
	if (selected == true) {
		FragColor = vec4(0.5, 0.1, 0.7, AddFog());
		return;
	}

	vec3 normal = normalize(vNCoords);
	vec3 viewDir = normalize(camPos - vFPos);

	vec3 result;

	vec3 difSample = vec3(texture(material.texture_diffuse1, vTCoords));
	vec3 specSample = vec3(texture(material.texture_specular1, vTCoords));

	for (int l = 0; l < LIGHTS; l++) {
		result += CalculatePointLight(light[l], normal, viewDir, difSample, specSample);
	}
	
	FragColor = vec4(vec3(result), AddFog());
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDir, vec3 difSample, vec3 specSample) {
	float distance = length(light.position - vFPos);
	float attenuation = 1.0 / (1.0 + light.attLinear * distance + light.attQuadratic * pow(distance, 2));

	vec3 lightDir = normalize(light.position - vFPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float angleDif = max(dot(lightDir, normal), 0.0);
	float angleSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * difSample;
	vec3 diffuse = light.diffuse * difSample * angleDif;
	vec3 specular = light.specular * specSample * angleSpec;

	vec3 result = attenuation * (ambient + diffuse + specular);

	return result;
}

float AddFog() {
	float dist = length(vFPos - camPos);
	float fogFactor = (MAX_FOG - dist) / (MAX_FOG - MIN_FOG);
	return clamp(fogFactor, 0.0, 1.0);
}
