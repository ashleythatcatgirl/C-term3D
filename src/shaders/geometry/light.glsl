#version 450 core

layout (points) in;
layout (line_strip, max_vertices = 256) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const float RADIUS = 0.5;
const int LIMIT = 32;

#define M_2PI 6.283

void CreateCircle(vec3 center, vec3 angleVector[LIMIT + 1]);

void main() {
	vec3 angleVectors[3][LIMIT + 1];
	vec3 center = gl_in[0].gl_Position.xyz;

	float angle;
	float angleSin, angleCos;
	for (int i = 0; i <= LIMIT; i++) {
		angle = M_2PI * float(i) / float(LIMIT);
		angleSin = sin(angle);
		angleCos = cos(angle);

		angleVectors[0][i] = vec3(0.0, angleSin, angleCos);
		angleVectors[1][i] = vec3(angleCos, 0.0, angleSin);
		angleVectors[2][i] = vec3(angleSin, angleCos, 0.0);
	}

	CreateCircle(center, angleVectors[0]);
	CreateCircle(center, angleVectors[1]);
	CreateCircle(center, angleVectors[2]);
}

void CreateCircle(vec3 center, vec3 angleVector[LIMIT + 1]) {
	vec3 off;
	for (int i = 0; i <= LIMIT; i++) {
		off = angleVector[i] * RADIUS;
		gl_Position = projection * view * model * vec4(center + off, 1.0);
		EmitVertex();
	}
	EndPrimitive();
}
