#include "Library/ibl_lighting.glsl"

const int DirLightCascadeMapCount = 3;
const int MaxDirLightCount = 4;

struct DirLight
{
	mat4 transform[DirLightCascadeMapCount];
	vec4 color;
	vec3 direction;
};

float calcShadowFactorCascade(vec4 position, DirLight light, sampler2D shadowMaps[MaxDirLightCount * DirLightCascadeMapCount], int samplerIndex, int pcfDistance)
{
	float totalFactor = 1.0f;
	for (int i = 0; i < DirLightCascadeMapCount; i++)
	{
		vec4 fragLightSpace = light.transform[i] * position;
		float shadowFactor = calcShadowFactor2D(fragLightSpace, shadowMaps[DirLightCascadeMapCount * samplerIndex + i], 0.005f, pcfDistance);
		shadowFactor = min(shadowFactor * float(i + 1), 1.0f);
		totalFactor *= shadowFactor;
	}
	return totalFactor;
}