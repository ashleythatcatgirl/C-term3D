#version 450 core

out vec4 FragColor;

in vec3 position;
uniform vec3 camPos;

const float sCellSize = 1.0;
const float lCellSize = 10.0;

const float sCellLine = 0.01;
const float lCellLine = 0.05;

const float axisLine = 0.05;

const vec4 sCellColor = vec4(0.3, 0.3, 0.3, 1.0);
const vec4 lCellColor = vec4(0.5, 0.5, 0.5, 1.0);

const vec4 xColor = vec4(0.8, 0.1, 0.4, 1.0);
const vec4 zColor = vec4(0.1, 0.4, 0.8, 1.0);

const float minFog = 96.0;
const float maxFog = 128.0;

const float camDist1 = 10.0;
const float camDist2 = 25.0;
const float camDist3 = 50.0;

void main() {
	float a_sCellSize = sCellSize;
	float a_lCellSize = lCellSize;
	float a_sCellLine = sCellLine;
	float a_lCellLine = lCellLine;
	float a_axisLine = axisLine;

	if (camPos.y >= camDist3) {
		a_sCellSize *= 10.0; 
		a_lCellSize *= 10.0; 
		a_sCellLine *= 10.0;
		a_lCellLine *= 10.0;
		a_axisLine *= 10.0; 
	} else if (camPos.y >= camDist2) {
		a_sCellSize *= 5.0; 
		a_lCellSize *= 5.0; 
		a_sCellLine *= 5.0;
		a_lCellLine *= 5.0;
		a_axisLine *= 5.0; 
	} else if (camPos.y >= camDist1) {
		a_sCellSize *= 2.0; 
		a_lCellSize *= 2.0; 
		a_sCellLine *= 2.0;
		a_lCellLine *= 2.0;
		a_axisLine *= 2.0; 
	}

	float ah_sCellLine = a_sCellLine / 2.0;
	float ah_lCellLine = a_lCellLine / 2.0;
	float ah_axisLine = a_axisLine / 2.0;

	float xs = mod((position.x + ah_sCellLine), a_sCellSize);
	float zs = mod((position.z + ah_sCellLine), a_sCellSize);
	float xl = mod((position.x + ah_lCellLine), a_lCellSize);
	float zl = mod((position.z + ah_lCellLine), a_lCellSize);

	vec4 color;

	if (xl <= a_lCellLine || zl <= a_lCellLine) {
		color = lCellColor;
	} else if (xs <= a_sCellLine || zs <= a_sCellLine) {
		color = sCellColor;
	} else {
		color = vec4(0.0, 0.0, 0.0, 0.0);
	}

	if (abs(position.z) <= ah_axisLine) {
		color = xColor;
	}
	if (abs(position.x) <= ah_axisLine) {
		color = zColor;
	}

	float dist = length(position - camPos);
	float fogFactor = (maxFog - dist) / (maxFog - minFog);
	color.a -= (1.0 - clamp(fogFactor, 0.0, 1.0));

	FragColor = color;
}

