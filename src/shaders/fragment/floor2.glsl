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

void ChangeCellSize();
vec4 AddGrid(float xl, float zl, float xs, float zs, float a_lCellLine, float a_sCellLine);
vec4 AddAxis(vec4 color, float a_axisLine);
float AddFog();

void main() {
	float a_sCellSize = sCellSize;
	float a_lCellSize = lCellSize;
	float a_sCellLine = sCellLine;
	float a_lCellLine = lCellLine;
	float a_axisLine = axisLine;

	if (camPos.y >= camDist3) {
		a_sCellSize *= 10.0; 
		a_lCellSize *= 10.0; 
		a_sCellLine *= 5.0;
		a_lCellLine *= 5.0;
		a_axisLine *= 5.0; 
	} else if (camPos.y >= camDist2) {
		a_sCellSize *= 5.0; 
		a_lCellSize *= 5.0; 
		a_sCellLine *= 2.5;
		a_lCellLine *= 2.5;
		a_axisLine *= 2.5; 
	} else if (camPos.y >= camDist1) {
		a_sCellSize *= 2.0; 
		a_lCellSize *= 2.0; 
	}

	float xs = mod((position.x + a_sCellLine), a_sCellSize);
	float zs = mod((position.z + a_sCellLine), a_sCellSize);
	float xl = mod((position.x + a_lCellLine), a_lCellSize);
	float zl = mod((position.z + a_lCellLine), a_lCellSize);

	vec4 color;

	color = AddGrid(xl, zl, xs, zs, a_lCellLine, a_sCellLine);
	color = AddAxis(color, a_axisLine);

	color.a -= AddFog();

	FragColor = color;
}

void ChangeCellSize() {

}

vec4 AddGrid(float xl, float zl, float xs, float zs, float a_lCellLine, float a_sCellLine) {
	if (xl <= a_lCellLine || zl <= a_lCellLine) {
		return lCellColor;
	} else if (xs <= a_sCellLine || zs <= a_sCellLine) {
		return sCellColor;
	}

	return vec4(0.0, 0.0, 0.0, 0.0);
}
vec4 AddAxis(vec4 color, float a_axisLine) {
	if (abs(position.z) <= a_axisLine) {
		return xColor;
	}
	if (abs(position.x) <= a_axisLine) {
		return zColor;
	}

	return color;
}

float AddFog() {
	float dist = length(position - camPos);
	float fogFactor = (maxFog - dist) / (maxFog - minFog);
	return (1.0 - clamp(fogFactor, 0.0, 1.0));
}
