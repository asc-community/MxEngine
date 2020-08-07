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

vec3 getDisplacement(vec2 uv, sampler2D heightMap, float displacementFactor)
{
	if (texCoord.x < 0.001f || texCoord.y < 0.001f || texCoord.x > 0.999f || texCoord.y > 0.999f)
		return vec3(0.0f);
	vec3 heightTex = texture(heightMap, uv).rgb;
	return displacementFactor * heightTex;
}

void main()
{
	vec4 modelPos = model * position;

	vec3 normalObjectSpace = normalMatrix * normal;
	modelPos.xyz += normalObjectSpace * getDisplacement(texCoord, map_height, displacement);

	gl_Position = ViewProjMatrix * modelPos;
	vsout.TexCoord = texCoord.xy;
	vsout.RenderColor = renderColor;
}

)