#version 450 core

out vec4 FragColor;

in vec3 position;

uniform vec3 camPos;

const vec3 COLOR_SMALL = vec3(0.3);
const vec3 COLOR_LARGE = vec3(0.5);

const vec3 COLOR_X = vec3(0.7, 0.2, 0.3);
const vec3 COLOR_Z = vec3(0.2, 0.3, 0.7);

const float SMALL_SCALE = 1.0;
const float LARGE_SCALE = 10.0;

const float SMALL_WIDTH = 0.1;
const float LARGE_WIDTH = 0.5;

const float MIN_FOG = 64.0;
const float MAX_FOG = 128.0;

const float CAMERA_DISTANCE1 = 10.0;
const float CAMERA_DISTANCE2 = 25.0;
const float CAMERA_DISTANCE3 = 50.0;

float ScaleSize(float size) {
	if (camPos.y >= CAMERA_DISTANCE3) {
		return size * 10.0;
	} else if (camPos.y >= CAMERA_DISTANCE2) {
		return size * 5.0;
	} else if (camPos.y >= CAMERA_DISTANCE1) {
		return size * 2.0;
	}

	return size;
}

float gridLine(vec2 coord, float scale, float width) {
    vec2 grid = abs(fract(coord / scale - 0.5) - 0.5) / fwidth(coord / scale);

    float line = min(grid.x, grid.y);

    return 1.0 - smoothstep(width, width + 1.0, line);
}

float axisLine(float value) {
    float dist = abs(value) / fwidth(value);

    return 1.0 - smoothstep(1.0, 2.0, dist);
}

float fogFactor() {
    float dist = length(position - camPos);

    return 1.0 - smoothstep(MIN_FOG, MAX_FOG, dist);
}

void main() {
    vec2 coord = position.xz;

    float smallGrid = gridLine(coord, ScaleSize(SMALL_SCALE), SMALL_WIDTH);
    float largeGrid = gridLine(coord, ScaleSize(LARGE_SCALE), LARGE_WIDTH);

    float xAxis = axisLine(position.z);
    float zAxis = axisLine(position.x);

    vec3 color = vec3(0.0);

    color = mix(color, COLOR_SMALL, smallGrid);
    color = mix(color, COLOR_LARGE, largeGrid);

    color = mix(color, COLOR_X, xAxis);
    color = mix(color, COLOR_Z, zAxis);

    float alpha = max(max(smallGrid, largeGrid), max(xAxis, zAxis));

    alpha *= fogFactor();

    FragColor = vec4(color, alpha);
}
