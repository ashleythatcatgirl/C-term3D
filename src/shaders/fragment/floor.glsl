#version 450 core

out vec4 FragColor;

in vec3 position;

uniform vec3 camPos;

const vec3 COLOR_SMALL = vec3(0.2);
const vec3 COLOR_MEDIUM = vec3(0.3);
const vec3 COLOR_LARGE = vec3(0.4);

const vec3 COLOR_X = vec3(0.7, 0.2, 0.4);
const vec3 COLOR_Z = vec3(0.2, 0.4, 0.7);

const float SMALL_SCALE = 1.0;
const float MEDIUM_SCALE = 10.0;
const float LARGE_SCALE = 100.0;

const float WIDTH = 0.1;

const float MIN_FOG = 96.0;
const float MAX_FOG = 128.0;

const float CAMERA_DISTANCE1 = 25.0;
const float CAMERA_DISTANCE2 = 50.0;
const float CAMERA_DISTANCE3 = 100.0;

float GridLine(vec2 coord, float scale, float width);
float AxisLine(float value);
float FogFactor();

void main() {
	vec2 coord = position.xz;
	float camHeight = camPos.y;

	float fade = smoothstep(10.0, CAMERA_DISTANCE1, abs(camHeight));

	float smallGrid = GridLine(coord, SMALL_SCALE, WIDTH) * (1.0 - fade);
	float mediumGrid = GridLine(coord, MEDIUM_SCALE, WIDTH);
	float largeGrid = GridLine(coord, LARGE_SCALE, WIDTH) * fade;

	float xAxis = AxisLine(position.z);
	float zAxis = AxisLine(position.x);

	vec3 color = vec3(0.0);

	color = mix(color, COLOR_SMALL, smallGrid);
	color = mix(color, COLOR_MEDIUM, mediumGrid);
	color = mix(color, COLOR_LARGE, largeGrid);

	color = mix(color, COLOR_X, xAxis);
	color = mix(color, COLOR_Z, zAxis);

	float alpha = max(max(max(smallGrid, mediumGrid), largeGrid), max(xAxis, zAxis));
	
	float facing = abs(dot(normalize(camPos - position), vec3(0,1,0)));
	alpha *= (facing * 2.0);
	alpha *= FogFactor();

	FragColor = vec4(color, alpha);
}

float GridLine(vec2 coord, float scale, float width) {
	vec2 grid = abs(fract(coord / scale - 0.5) - 0.5) / fwidth(coord / scale);
	float line = min(grid.x, grid.y);

	return 1.0 - smoothstep(width, width + 1.0, line);
}

float AxisLine(float value) {
	float dist = abs(value) / fwidth(value);

	return 1.0 - smoothstep(WIDTH, WIDTH + 1.0, dist);
}

float FogFactor() {
	float dist = length(position - camPos);

	return 1.0 - smoothstep(MIN_FOG, MAX_FOG, dist);
}
