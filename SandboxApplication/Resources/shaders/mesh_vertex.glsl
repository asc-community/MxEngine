#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 offset;

uniform mat4 MVP;

out vec4 vertexColor;

void main()
{
	vertexColor = vec4(0, 1, 0, 0);
	gl_Position = MVP * (position + vec4(offset, 0));
}