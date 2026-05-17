#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vVertexCoords;
in vec3 vNormalCoords;
in vec2 vTextureCoords;
in vec3 vFragmentPos;

out vec3 gVertexCoords;
out vec3 gNormalCoords;
out vec2 gTextureCoords;
out vec3 gFragmentPos;

void main() {
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();

	gVCoords = vVCoords;
	gNCoords = vNCoords;
	gTCoords = vTCoords;
	gFPos = vFPos;
}
