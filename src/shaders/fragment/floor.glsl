#version 450 core

out vec4 FragColor;

in vec3 position;
uniform vec3 camPos;

const float fadingS = 0.125;
const float fadingL = 0.25;

const float sizeS = 1.0;
const float sizeL = 10.0;

const float widthS = 0.5;
const float widthL = 1.0;

const vec3 colorS = vec3(0.3, 0.3, 0.3);
const vec3 colorL = vec3(0.5, 0.5, 0.5);

const vec3 colorX = vec3(0.7, 0.2, 0.3);
const vec3 colorZ = vec3(0.2, 0.3, 0.7);

const float camDist1 = 10.0;
const float camDist2 = 25.0;
const float camDist3 = 50.0;

const float minFog = 64.0;
const float maxFog = 128.0;

float Fog();

void main() {
	vec3 color = vec3(0.0);
	float alpha = 0.0;

	float altSizeS = sizeS;
	float altSizeL = sizeL;

	if (camPos.y >= camDist3) {
		altSizeS *= 10.0;
		altSizeL *= 10.0;
	} else if (camPos.y >= camDist2) {
		altSizeS *= 5.0;
		altSizeL *= 5.0;
	} else if (camPos.y >= camDist1) {
		altSizeS *= 2.0;
		altSizeL *= 2.0;
	}

	float dist = length(position - camPos);
	float distFactor = (maxFog - dist) / (maxFog);
	float altDistFactor = (1 - clamp(distFactor, 0.0, 0.9));
	float altWidthS = altDistFactor * widthS;
	float altWidthL = altDistFactor * widthL;
	float altFadingL = altDistFactor * fadingL;

	float xModS = mod(position.x + altWidthS / 2.0, altSizeS);
	float zModS = mod(position.z + altWidthS / 2.0, altSizeS);
	float xModL = mod(position.x + altWidthL / 2.0, altSizeL);
	float zModL = mod(position.z + altWidthL / 2.0, altSizeL);

	float xMod = mod(abs(position.x), 0.0) * 2.0;
	float zMod = mod(abs(position.z), 0.0) * 2.0;

	if (xModL <= altWidthL || zModL <= altWidthL) {
		color = colorL;
		alpha = 1.0;
	} else if (xModS <= altWidthS || zModS <= altWidthS) {
		color = colorS;
		alpha = 1.0;
	}

	if (zMod <= altWidthL) {
		color = colorX;
		alpha = 1.0;
	} else if (xMod <= altWidthL) {
		color = colorZ;
		alpha = 1.0;
	}

	alpha -= Fog();

	FragColor = vec4(color, alpha);
}

float Fog() {
	float dist = length(position - camPos);
	float fogFactor = (maxFog - dist) / (maxFog - minFog);
	return (1.0 - clamp(fogFactor, 0.0, 1.0));
}
