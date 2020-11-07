#include "Library/lighting.glsl"

const int DirLightCascadeMapCount = 3;

struct DirLight
{
	mat4 transform[DirLightCascadeMapCount];
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

vec3 calcColorUnderDirLight(FragmentInfo fragment, DirLight light, vec3 viewDir, float shadowFactor)
{
	return calculateLighting(fragment, viewDir, light.direction, light.ambient, light.diffuse, light.specular, shadowFactor);
}

float calcShadowFactorCascade(vec4 position, DirLight light, sampler2D shadowMaps[DirLightCascadeMapCount], int pcfDistance)
{
	float totalFactor = 1.0f;
	for (int i = 0; i < DirLightCascadeMapCount; i++)
	{
		vec4 fragLightSpace = light.transform[i] * position;
		float shadowFactor = calcShadowFactor2D(fragLightSpace, shadowMaps[i], 0.005f, pcfDistance);
		shadowFactor = min(shadowFactor * float(i + 1), 1.0f);
		totalFactor = (totalFactor == 1.0f ? shadowFactor : totalFactor * shadowFactor);
	}
	return totalFactor;
}