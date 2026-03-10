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

in vec3 fragPos;
uniform vec3 lightPos;
uniform vec3 camPos;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = lightColor * ambientStrength;


	vec3 lightDir = normalize(lightPos - fragPos);

	vec3 normalized = normalize(normalCords);
	float angleDif = max(dot(lightDir, normalized), 0.0);
	vec3 diffuse = lightColor * angleDif;

	
	float specularStrength = 0.5;
	int shine = 64;
	vec3 viewDir = normalize(camPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normalized);

	float angleSpec = pow(max(dot(viewDir, reflectDir), 0.0), shine);
	vec3 specular = lightColor * angleSpec * specularStrength;


	//FragColor = vec4(ambient * objectColor, 1.0);
	FragColor = vec4((ambient + diffuse + specular) * vec3(texture(texture0, textureCords)), 1.0);
}
