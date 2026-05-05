#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 64) out;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 camPos;

out vec3 position;

void main() {
	vec3 center = gl_in[0].gl_Position.xyz;
	vec3 off[4] = {
		{ 50.0, 0.0,  50.0},
		{-50.0, 0.0,  50.0},
		{ 50.0, 0.0, -50.0},
		{-50.0, 0.0, -50.0}
	};

	for (int i = 0; i < 4; i++) {

		gl_Position = projection * view * vec4(center + off[i], 1.0);
		position = vec3(center + off[i]);
		EmitVertex();
	}
	EndPrimitive();
}
