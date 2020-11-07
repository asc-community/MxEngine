#include "Library/shader_utils.glsl"

vec3 calculateLighting(FragmentInfo fragment, vec3 viewDirection,
    vec3 lightDirection, vec3 lightAmbient, vec3 lightDiffuse, vec3 lightSpecular, float shadowFactor)
{
	vec3 lightDir = normalize(lightDirection);
	vec3 Hdir = normalize(lightDir + viewDirection);

	float diffuseCoef = max(dot(lightDir, fragment.normal), 0.0f);
	float specularCoef = pow(max(dot(Hdir, fragment.normal), 0.0f), fragment.specularIntensity);

	vec3 ambientColor = fragment.albedo;
	vec3 diffuseColor = fragment.albedo * diffuseCoef;
	vec3 specularColor = vec3(specularCoef * fragment.specularFactor);

	ambientColor = ambientColor * lightAmbient;
	diffuseColor = diffuseColor * lightDiffuse;
	specularColor = specularColor * lightSpecular;

	return vec3(ambientColor + shadowFactor * (specularColor + diffuseColor));
}