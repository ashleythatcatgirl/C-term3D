#version 450 core

out vec4 FragColor;

in vec3 position;

const float cellSize = 10.0;
const vec4 darkColor = vec4(0.04, 0.03, 0.05, 1.0);
const vec4 lightColor = vec4(0.5, 0.5, 0.5, 1.0);

const vec4 xColor = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 zColor = vec4(0.0, 0.0, 1.0, 1.0);

void main() {
	float x = mod(position.x, cellSize);
	float z = mod(position.z, cellSize);

	vec4 color;

	if (x <= 0.1 || z <= 0.1) {
		color = lightColor;
	} else {
		color = darkColor;
	}

	if (abs(position.z) < 0.1) {
		color = xColor;
	}
	if (abs(position.x < 0.1) {
		color = zColor;
	}

	FragColor = color;
}

