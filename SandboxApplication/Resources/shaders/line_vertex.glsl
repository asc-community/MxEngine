#version 330 core

layout(location = 0) in vec4 position;

out vec4 vertexColor;
uniform mat4 MVP;

void main()
{
	vertexColor = vec4(1.0);
	gl_Position = MVP * position;
}