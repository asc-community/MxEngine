#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 offset;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * (position + vec4(offset, 0));
}