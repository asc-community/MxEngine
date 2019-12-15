#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 instanceTranslation;

uniform mat4 MVP;

out vec4 vertexColor;

void main()
{
	vertexColor = color;
	gl_Position = MVP * (position + vec4(instanceTranslation, 0));
}