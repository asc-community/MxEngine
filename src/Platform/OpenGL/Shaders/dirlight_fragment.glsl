#include "Library/directional_light.glsl"

out vec4 OutColor;
in vec2 TexCoord;

struct Camera
{
	vec3 position;
	mat4 invViewProjMatrix;
	mat4 viewProjMatrix;
};

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

uniform int lightCount;
uniform int pcfDistance;
uniform int lightSamples;
uniform Camera camera;

uniform sampler2D HDRTex;

uniform DirLight lights[MaxDirLightCount];
uniform sampler2D lightDepthMaps[MaxDirLightCount * DirLightCascadeMapCount];

void main()
{
	FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
	vec3 objectColor = texture(HDRTex, TexCoord).rgb;
	
	vec3 totalColor = objectColor;
	for (int i = 0; i < lightCount; i++)
	{
		vec4 pos = vec4(fragment.position, 1.0f);
		float shadowFactor = calcShadowFactorCascade(pos, lights[i], lightDepthMaps, i, pcfDistance);
		totalColor += calculateLighting(objectColor, fragment.normal, lights[i].direction, lights[i].color.rgb, lights[i].color.a, shadowFactor);
	}
	
	OutColor = vec4(totalColor, 1.0f);
}