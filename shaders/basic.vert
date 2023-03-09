#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() 
{
	
	//fPosition = vPosition; -- eredeti
	//fPosition = view * model * vec4(vPosition, 1.0f);
	fNormal = vNormal;
	fPosition = vec3(model* vec4(vPosition,1.0f));
	fTexCoords = vTexCoords;
	
	gl_Position = projection * view * model * vec4(vPosition, 50.0f);
}
