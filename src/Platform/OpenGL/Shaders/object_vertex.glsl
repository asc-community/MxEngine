#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
#define MAX_POINT_LIGHTS 2
#define MAX_SPOT_LIGHTS 8
#define MAX_DIR_LIGHTS 2
)" \
MAKE_STRING(

layout(location = 0)  in vec4 position;
layout(location = 1)  in vec2 texCoord;
layout(location = 2)  in vec3 normal;
layout(location = 3)  in vec3 tangent;
layout(location = 4)  in vec3 bitangent;
layout(location = 5)  in mat4 model;
layout(location = 9)  in mat3 normalMatrix;
layout(location = 12) in vec3 renderColor;

uniform mat4 ViewProjMatrix;
uniform mat4 DirLightProjMatrix[MAX_DIR_LIGHTS];
uniform mat4 SpotLightProjMatrix[MAX_SPOT_LIGHTS];
uniform int pointLightCount;
uniform int spotLightCount;
uniform int dirLightCount;
uniform float displacement;
uniform sampler2D map_height;

out VSout
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 FragPosWorld;
	vec3 RenderColor;
	vec4 FragPosDirLight[MAX_DIR_LIGHTS];
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
	vsout.Normal = N;

	modelPos.xyz += displacement * vsout.Normal * (texture(map_height, texCoord).rgb - vec3(0.5f)) * 2.0f;

	vsout.FragPosWorld = vec3(modelPos);
	vsout.RenderColor = renderColor;

	for (int i = 0; i < dirLightCount; i++)
	{
		vsout.FragPosDirLight[i] = DirLightProjMatrix[i] * modelPos;
	}

	for (int i = 0; i < spotLightCount; i++)
	{
		vsout.FragPosSpotLight[i] = SpotLightProjMatrix[i] * modelPos;
	}


	gl_Position = ViewProjMatrix * modelPos;
}

)