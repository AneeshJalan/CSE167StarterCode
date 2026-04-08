#version 330 core

uniform samplerCube cubeMap;

in vec3 vertTexCoord;
in vec3 fragNormal;

out vec4 fragColor;

void main() {
	fragColor = texture(cubeMap, vertTexCoord);
	
	//fragColor = vec4(fragNormal, 1.0f);
	
	//fragColor = vec4(0.0f, 1.0f, 0.0f, 0.0f);
}