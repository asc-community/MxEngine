#include "Library/shader_utils.glsl"

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
	vec3 lightDir = normalize(light.direction);
	vec3 Hdir = normalize(lightDir + viewDir);

	float diffuseCoef = max(dot(lightDir, fragment.normal), 0.0f);
	float specularCoef = pow(max(dot(Hdir, fragment.normal), 0.0f), fragment.specularIntensity);

	vec3 ambientColor = fragment.albedo;
	vec3 diffuseColor = fragment.albedo * diffuseCoef;
	vec3 specularColor = vec3(specularCoef * fragment.specularFactor);

	ambientColor = ambientColor * light.ambient;
	diffuseColor = diffuseColor * light.diffuse;
	specularColor = specularColor * light.specular;

	return vec3(ambientColor + shadowFactor * (specularColor + diffuseColor));
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