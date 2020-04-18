#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec4 normal;
layout(location = 3) in mat4 model;
layout(location = 7) in mat3 normalMatrix;

uniform mat4 ViewProjMatrix;

void main()
{
	gl_Position = ViewProjMatrix * model * position;
}

)