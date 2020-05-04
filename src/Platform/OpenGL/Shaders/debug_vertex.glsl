#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 ViewProjMatrix;

out vec4 FragmentColor;

void main()
{
	gl_Position = ViewProjMatrix * position;
	FragmentColor = color;
}

)