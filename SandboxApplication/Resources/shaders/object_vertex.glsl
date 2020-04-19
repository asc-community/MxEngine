#version 400 core
#define MAX_POINT_LIGHTS 2
#define MAX_SPOT_LIGHTS 8

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec3 texCoord;
layout(location = 2)  in vec4 normal;
layout(location = 3)  in mat4 model;
layout(location = 7)  in mat3 normalMatrix;
layout(location = 10) in vec4 renderColor;

uniform mat4 ViewProjMatrix;
uniform mat4 DirLightProjMatrix;
uniform mat4 SpotLightProjMatrix[MAX_SPOT_LIGHTS];
uniform int pointLightCount;
uniform int spotLightCount;

out VSout
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPosWorld;
	vec4 RenderColor;
	vec4 FragPosDirLight;
	vec4 FragPosSpotLight[MAX_SPOT_LIGHTS];
} vsout;

void main()
{
	vec4 modelPos = model * position;
	gl_Position = ViewProjMatrix * modelPos;

	vsout.TexCoord = texCoord.xy;
	vsout.Normal = normalMatrix * vec3(normal);
	vsout.FragPosWorld = vec3(modelPos);
	vsout.RenderColor = renderColor;
	vsout.FragPosDirLight = DirLightProjMatrix * modelPos;

	for (int i = 0; i < spotLightCount; i++)
	{
		vsout.FragPosSpotLight[i] = SpotLightProjMatrix[i] * modelPos;
	}
}