#version 400 core
#define MAX_POINT_LIGHTS 2
#define MAX_SPOT_LIGHTS 8

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec2 texCoord;
layout(location = 2)  in vec3 normal;
layout(location = 3)  in vec3 tangent;
layout(location = 4)  in vec3 bitangent;
layout(location = 5)  in mat4 model;
layout(location = 9)  in mat3 normalMatrix;
layout(location = 12) in vec4 renderColor;

uniform mat4 ViewProjMatrix;
uniform mat4 DirLightProjMatrix;
uniform mat4 SpotLightProjMatrix[MAX_SPOT_LIGHTS];
uniform int pointLightCount;
uniform int spotLightCount;
uniform vec3 displacement;
uniform sampler2D map_height;

out VSout
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPosWorld;
	vec4 RenderColor;
	vec4 FragPosDirLight;
	vec4 FragPosSpotLight[MAX_SPOT_LIGHTS];
	mat3 TBN;
} vsout;

void main()
{
	vec4 modelPos = model * position;
	vec3 T = normalize(vec3(normalMatrix * tangent));
	vec3 B = normalize(vec3(normalMatrix * bitangent));
	vec3 N = normalize(vec3(normalMatrix * normal));

	vsout.TBN = mat3(T, B, N);
	vsout.TexCoord = texCoord;
	vsout.Normal = normalMatrix * normal;

	vsout.FragPosWorld = vec3(modelPos);
	vsout.RenderColor = renderColor;
	vsout.FragPosDirLight = DirLightProjMatrix * modelPos;

	for (int i = 0; i < spotLightCount; i++)
	{
		vsout.FragPosSpotLight[i] = SpotLightProjMatrix[i] * modelPos;
	}

	modelPos.xyz += displacement * vsout.Normal * (texture(map_height, texCoord).rgb - vec3(0.5f)) * 2.0f;

	gl_Position = ViewProjMatrix * modelPos;
}