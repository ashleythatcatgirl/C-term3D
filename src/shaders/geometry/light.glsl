#version 450 core

layout (points) in;
layout (line_strip, max_vertices = 256) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const float radius = 0.5;
const float limit = 40.0;

void main() {
	vec3 center = gl_in[0].gl_Position.xyz;

	float angle;
	vec3 off;
	for (float i = 0.0; i <= limit; i++) {
		angle = 2 * 3.14 * i / limit;

		off = vec3(sin(angle), cos(angle), 0.0) * radius;
		gl_Position = projection * view * model * vec4(center + off, 1.0);
		EmitVertex();
	}
	EndPrimitive();
	for (float i = 0.0; i <= limit; i++) {
		angle = 2 * 3.14 * i / limit;

		off = vec3(0.0, sin(angle), cos(angle)) * radius;
		gl_Position = projection * view * model * vec4(center + off, 1.0);
		EmitVertex();
	}
	EndPrimitive();
	for (float i = 0.0; i <= limit; i++) {
		angle = 2 * 3.14 * i / limit;

		off = vec3(cos(angle), 0.0, sin(angle)) * radius;
		gl_Position = projection * view * model * vec4(center + off, 1.0);
		EmitVertex();
	}
	EndPrimitive();
}
