#version 450 core

out vec4 FragColor;

in vec3 position;

const float sCellSize = 10.0;
const float lCellSize = 50.0;

const float sCellLine = 0.05;
const float lCellLine = 0.1;

const float axisLine = 0.1;

const vec4 sCellColor = vec4(0.3, 0.3, 0.3, 1.0);
const vec4 lCellColor = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 darkColor = vec4(0.04, 0.03, 0.05, 1.0);

const vec4 xColor = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 zColor = vec4(0.0, 0.0, 1.0, 1.0);

void main() {
	float sHCellLine = sCellLine / 2.0;
	float lHCellLine = lCellLine / 2.0;
	float hAxisLine = axisLine / 2.0;

	float xs = mod((position.x + sHCellLine), sCellSize);
	float zs = mod((position.z + sHCellLine), sCellSize);
	float xl = mod((position.x + lHCellLine), lCellSize);
	float zl = mod((position.z + lHCellLine), lCellSize);

	vec4 color;

	if (xl <= lCellLine || zl <= lCellLine) {
		color = lCellColor;
	} else if (xs <= sCellLine || zs <= sCellLine) {
		color = sCellColor;
	} else {
		color = darkColor;
	}


	if (abs(position.z) <= hAxisLine) {
		color = xColor;
	}
	if (abs(position.x) <= hAxisLine) {
		color = zColor;
	}

	FragColor = color;
}

