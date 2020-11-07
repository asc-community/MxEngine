#include "Library/shader_utils.glsl"
#include "Library/pbr_lighting.glsl"

vec3 calculateLighting(FragmentInfo fragment, vec3 viewDirection, vec3 lightDirection, vec3 lightColor, float ambientFactor, float shadowFactor)
{
	vec3 normLightDirection = normalize(lightDirection);
	vec3 ggx = GGXCookTorrance(fragment.normal, normLightDirection, viewDirection, fragment.roughnessFactor, fragment.metallicFactor, fragment.albedo);
	return (fragment.albedo * ambientFactor + ggx * shadowFactor) * lightColor;
}