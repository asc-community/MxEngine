#define MAKE_STRING(...) #__VA_ARGS__
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
uniform float displacement;
uniform sampler2D map_height;

out VSout
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 RenderColor;
	mat3 TBN;
	vec3 Position;
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
	vec3 T = normalize(vec3(normalMatrix * tangent));
	vec3 B = normalize(vec3(normalMatrix * bitangent));
	vec3 N = normalize(vec3(normalMatrix * normal));

	vsout.TBN = mat3(T, B, N);
	vsout.TexCoord = texCoord;
	vsout.Normal = N;
	vsout.RenderColor = renderColor;

	modelPos.xyz += vsout.Normal * getDisplacement(texCoord, map_height, displacement);
	vsout.Position = modelPos.xyz;

	gl_Position = ViewProjMatrix * modelPos;
}

)