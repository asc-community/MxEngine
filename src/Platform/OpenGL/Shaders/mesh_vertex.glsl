#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

layout(location = 0) in vec4 position;
layout(location = 3) in mat4 model;

uniform mat4 ViewProjMatrix;

void main()
{
	gl_Position = ViewProjMatrix * model * position;
}

)