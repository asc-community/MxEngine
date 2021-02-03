#include "Library/lighting.glsl"

out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

in SpotLightInfo
{
	vec3 position;
	float innerAngle;
	vec3 direction;
	float outerAngle;
	vec4 color;
	float maxDistance;
} spotLight;

struct SpotLight
{
	vec3 position;
	float innerAngle;
	vec3 direction;
	float outerAngle;
	vec4 color;
	float maxDistance;
};

struct Camera
{
	vec3 position;
	mat4 invViewProjMatrix;
	mat4 viewProjMatrix;
};

uniform mat4 worldToLightTransform;
uniform bool castsShadows;
uniform sampler2D lightDepthMap;
uniform Camera camera;
uniform int pcfDistance;
uniform vec2 viewportSize;

vec3 calcColorUnderSpotLight(FragmentInfo fragment, SpotLight light, vec3 viewDirection, vec3 fragLightSpace, sampler2D map_shadow, bool computeShadow)
{
	vec3 lightPath = light.position - fragment.position;
	float lightDistance = length(lightPath);

	float shadowFactor = 1.0f;
	if (computeShadow) { shadowFactor = calcShadowFactor2D(fragLightSpace, map_shadow, 0.005f, pcfDistance); }

	float fragAngle = dot(normalize(lightPath), -light.direction);
	float epsilon = light.innerAngle - light.outerAngle;
	float angleIntensity = pow(clamp((fragAngle - light.outerAngle) / epsilon, 0.0f, 1.0f), 2.0f);
	float intensity = clamp(angleIntensity * angleIntensity / (lightDistance * lightDistance + 1.0f), 0.0f, 1.0f);
	intensity *= max(1.0 - pow(2.0 * lightDistance / light.maxDistance, 4.0), 0.0);

	return calculateLighting(fragment, viewDirection, lightPath, intensity * light.color.rgb, light.color.a, shadowFactor);
}

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / viewportSize;
	FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);

	float fragDistance = length(camera.position - fragment.position);
	vec3 viewDirection = normalize(camera.position - fragment.position);

	SpotLight light;
	light.position = spotLight.position;
	light.innerAngle = spotLight.innerAngle;
	light.direction = spotLight.direction;
	light.outerAngle = spotLight.outerAngle;
	light.color = spotLight.color;
	light.maxDistance = spotLight.maxDistance;

	vec4 fragLightSpace = worldToLightTransform * vec4(fragment.position, 1.0f);
	fragLightSpace.xyz /= fragLightSpace.w;
	vec3 totalColor = calcColorUnderSpotLight(fragment, light, viewDirection, fragLightSpace.xyz, lightDepthMap, castsShadows);

	OutColor = vec4(totalColor, 1.0f);
}