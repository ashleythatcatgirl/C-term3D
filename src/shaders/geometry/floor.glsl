#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 64) out;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 camPos;

out vec3 position;

const float FLOOR_SIZE = 128.0;

void main() {
	vec3 center = gl_in[0].gl_Position.xyz;
	vec3 off[4] = {
		{ FLOOR_SIZE, 0.0,  FLOOR_SIZE},
		{-FLOOR_SIZE, 0.0,  FLOOR_SIZE},
		{ FLOOR_SIZE, 0.0, -FLOOR_SIZE},
		{-FLOOR_SIZE, 0.0, -FLOOR_SIZE}
	};

	vec3 vertexPos;
	for (int i = 0; i < 4; i++) {
		vertexPos = vec3(center + off[i] + vec3(camPos.x, 0.0, camPos.z));

		gl_Position = projection * view * vec4(vertexPos, 1.0);
		position = vertexPos;
		EmitVertex();
	}
	EndPrimitive();
}
