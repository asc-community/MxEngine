#include "Library/shader_utils.glsl"
#include "Library/pbr_lighting.glsl"

vec3 calculateLighting(FragmentInfo fragment, vec3 viewDirection, vec3 lightDirection, vec3 lightColor, float ambientFactor, float shadowFactor)
{
	vec3 normLightDirection = normalize(lightDirection);

	// return GGXCookTorrance(fragment.normal, normLightDirection, viewDirection, 1.0f - fragment.specularFactor, fragment.albedo);

	vec3 Hdir = normalize(normLightDirection + viewDirection);
	
	float diffuseCoef = max(dot(normLightDirection, fragment.normal), 0.0f);
	float specularCoef = pow(max(dot(Hdir, fragment.normal), 0.0f), fragment.specularIntensity);
	
	vec3 ambientColor = fragment.albedo * ambientFactor;
	vec3 diffuseColor = fragment.albedo * diffuseCoef;
	vec3 specularColor = vec3(specularCoef * fragment.specularFactor);
	
	ambientColor  = ambientColor  * lightColor;
	diffuseColor  = diffuseColor  * lightColor;
	specularColor = specularColor * lightColor;
	
	return vec3(ambientColor + shadowFactor * (specularColor + diffuseColor));
}