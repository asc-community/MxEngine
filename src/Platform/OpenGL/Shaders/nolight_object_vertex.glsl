#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec2 texCoord;
layout(location = 2)  in vec3 normal;
layout(location = 5)  in mat4 model;
layout(location = 9)  in mat3 normalMatrix;
layout(location = 12) in vec3 renderColor;

uniform mat4 ViewProjMatrix;
uniform float displacement;
uniform sampler2D map_height;

out VSout
{
	vec2 TexCoord;
	vec3 RenderColor;
} vsout;

void main()
{
	vec4 modelPos = model * position;

	vec3 normalSpace = normalMatrix * normal;
	modelPos.xyz += displacement * normalSpace * (texture(map_height, texCoord).rgb - vec3(0.5f)) * 2.0f;

	gl_Position = ViewProjMatrix * modelPos;
	vsout.TexCoord = texCoord.xy;
	vsout.RenderColor = renderColor;
}

)