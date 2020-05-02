#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec3 texCoord;
layout(location = 5)  in mat4 model;
layout(location = 12) in vec4 renderColor;

uniform mat4 ViewProjMatrix;

out VSout
{
	vec2 TexCoord;
	vec4 RenderColor;
} vsout;

void main()
{
	vec4 modelPos = model * position;
	gl_Position = ViewProjMatrix * modelPos;
	vsout.TexCoord = texCoord.xy;
	vsout.RenderColor = renderColor;
}

)