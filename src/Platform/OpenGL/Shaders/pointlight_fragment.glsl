#define MAKE_STRING(...) #__VA_ARGS__

#include "shader_utils.glsl"
MAKE_STRING(

out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

in PointLightInfo
{
	vec3 position;
	float radius;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} pointLight;

struct PointLight
{
	vec3 position;
	float radius;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Camera
{
	vec3 position;
	mat4 invProjMatrix;
	mat4 invViewMatrix;
	mat4 viewProjMatrix;
};

uniform samplerCube lightDepthMap;
uniform bool castsShadows;
uniform Camera camera;
uniform int pcfDistance;
uniform vec2 viewportSize;

vec3 calcColorUnderPointLight(FragmentInfo fragment, PointLight light, vec3 viewDir, samplerCube map_shadow, bool computeShadow)
{
	vec3 lightPath = light.position - fragment.position;
	float lightDistance = length(lightPath);

	if (light.radius < lightDistance || isnan(lightDistance))
		discard;

	vec3 lightDir = normalize(lightPath);
	vec3 Hdir = normalize(lightDir + viewDir);

	float shadowFactor = 1.0f;
	if (computeShadow) { shadowFactor = CalcShadowFactor3D(lightPath, viewDir, light.radius, 0.15f, map_shadow); }

	float intensity = (light.radius - lightDistance) / light.radius;

	float diffuseCoef = max(dot(lightDir, fragment.normal), 0.0f);
	float specularCoef = pow(clamp(dot(Hdir, fragment.normal), 0.0f, 1.0f), fragment.specularIntensity);

	vec3 ambientColor = fragment.albedo;
	vec3 diffuseColor = fragment.albedo * diffuseCoef;
	vec3 specularColor = vec3(specularCoef * fragment.specularFactor);

	ambientColor  = ambientColor  * intensity * light.ambient;
	diffuseColor  = diffuseColor  * intensity * light.diffuse;
	specularColor = specularColor * intensity * light.specular;

	return vec3(ambientColor + shadowFactor * (diffuseColor + specularColor));
}

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / viewportSize;
	FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewMatrix, camera.invProjMatrix);

	float fragDistance = length(camera.position - fragment.position);
	vec3 viewDirection = normalize(camera.position - fragment.position);

	PointLight light;
	light.position = pointLight.position;
	light.radius = pointLight.radius;
	light.ambient = pointLight.ambient;
	light.diffuse = pointLight.diffuse;
	light.specular = pointLight.specular;

	vec3 totalColor = vec3(0.0f);
	totalColor += calcColorUnderPointLight(fragment, light, viewDirection, lightDepthMap, castsShadows);

	OutColor = vec4(totalColor, 1.0f);
}

)