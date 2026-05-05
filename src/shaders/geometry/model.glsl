#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vVCoords;
in vec3 vNCoords;
in vec2 vTCoords;
in vec3 vFPos;

out vec3 gVCoords;
out vec3 gNCoords;
out vec2 gTCoords;
out vec3 gFPos;

void main() {
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();

	gVCoords = vVCoords;
	gNCoords = vNCoords;
	gTCoords = vTCoords;
	gFPos = vFPos;
}
