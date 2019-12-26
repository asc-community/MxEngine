#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec4 normal;

uniform mat4 MVP;

out vec2 TexCoord;

void main()
{
	gl_Position = MVP * position;
	TexCoord = texCoord.xy;
}