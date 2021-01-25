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
	vec3 projectedPositions[DirLightCascadeMapCount];
	float shadowFactors[DirLightCascadeMapCount + 1];
	shadowFactors[DirLightCascadeMapCount] = 1.0;

	for (int i = 0; i < DirLightCascadeMapCount; i++)
	{
		vec4 fragLightSpace = light.transform[i] * position;
		projectedPositions[i] = fragLightSpace.xyz / fragLightSpace.w;
		shadowFactors[i] = calcShadowFactor2D(projectedPositions[i], shadowMaps[DirLightCascadeMapCount * samplerIndex + i], 0.005f, pcfDistance);
	}

	float totalFactor = 1.0f;

	for (int i = 0; i < DirLightCascadeMapCount; i++)
	{
		vec3 pos = projectedPositions[i];
		vec2 normCoords = abs(2.0 * pos.xy - 1.0);
		normCoords = clamp(10.0 * normCoords - 9.0, 0.0, 1.0);
		float mixCoef = max(normCoords.x, normCoords.y);
		float currentFactor = mix(shadowFactors[i], shadowFactors[i + 1], mixCoef);

		currentFactor = min(currentFactor * float(i + 1), 1.0f);
		totalFactor *= currentFactor;
	}

	return totalFactor;
}