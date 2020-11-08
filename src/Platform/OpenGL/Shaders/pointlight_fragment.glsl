#include "Library/lighting.glsl"

out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

in PointLightInfo
{
	vec3 position;
	float radius;
	vec4 color;
} pointLight;

struct PointLight
{
	vec3 position;
	float radius;
	vec4 color;
};

struct Camera
{
	vec3 position;
	mat4 invViewProjMatrix;
	mat4 viewProjMatrix;
};

uniform samplerCube lightDepthMap;
uniform bool castsShadows;
uniform Camera camera;
uniform int pcfDistance;
uniform int lightSamples;
uniform vec2 viewportSize;

uniform EnvironmentInfo environment;

vec3 calcColorUnderPointLight(FragmentInfo fragment, PointLight light, vec3 viewDir, samplerCube map_shadow, bool computeShadow)
{
	vec3 lightPath = light.position - fragment.position;
	float lightDistance = length(lightPath);

	if (light.radius < lightDistance || isnan(lightDistance))
		discard;

	float shadowFactor = 1.0f;
	if (computeShadow) { shadowFactor = CalcShadowFactor3D(lightPath, viewDir, light.radius, 0.15f, map_shadow); }

	float intensity = (light.radius - lightDistance) / light.radius;

	return calculateLighting(fragment, viewDir, lightPath, environment, lightSamples, intensity * light.color.rgb, light.color.a, shadowFactor);
}

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / viewportSize;
	FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);

	float fragDistance = length(camera.position - fragment.position);
	vec3 viewDirection = normalize(camera.position - fragment.position);

	PointLight light;
	light.position = pointLight.position;
	light.radius = pointLight.radius;
	light.color = pointLight.color;

	vec3 totalColor = vec3(0.0f);
	totalColor += calcColorUnderPointLight(fragment, light, viewDirection, lightDepthMap, castsShadows);

	OutColor = vec4(totalColor, 1.0f);
}