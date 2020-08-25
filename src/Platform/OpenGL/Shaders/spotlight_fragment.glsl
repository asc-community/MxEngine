#include "shader_utils.glsl"
EMBEDDED_SHADER(

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
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} spotLight;

struct SpotLight
{
	vec3 position;
	float innerAngle;
	vec3 direction;
	float outerAngle;
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

uniform mat4 worldToLightTransform;
uniform bool castsShadows;
uniform sampler2D lightDepthMap;
uniform Camera camera;
uniform int pcfDistance;
uniform vec2 viewportSize;

vec3 calcColorUnderSpotLight(FragmentInfo fragment, SpotLight light, vec3 viewDir, vec4 fragLightSpace, sampler2D map_shadow, bool computeShadow)
{
	vec3 lightDir = normalize(light.position - fragment.position);
	vec3 Hdir = normalize(lightDir + viewDir);

	float shadowFactor = 1.0f;
	if (computeShadow) { shadowFactor = calcShadowFactor2D(fragLightSpace, map_shadow, 0.005f, pcfDistance); }

	float diffuseCoef = max(dot(lightDir, fragment.normal), 0.0f);
	float specularCoef = pow(clamp(dot(Hdir, fragment.normal), 0.0f, 1.0f), fragment.specularIntensity);

	float fragAngle = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerAngle - light.outerAngle;
	float intensity = clamp((fragAngle - light.outerAngle) / epsilon, 0.0f, 1.0f);

	vec3 ambientColor  = fragment.albedo;
	vec3 diffuseColor  = fragment.albedo * diffuseCoef;
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

	SpotLight light;
	light.position = spotLight.position;
	light.innerAngle = spotLight.innerAngle;
	light.direction = spotLight.direction;
	light.outerAngle = spotLight.outerAngle;
	light.ambient = spotLight.ambient;
	light.diffuse = spotLight.diffuse;
	light.specular = spotLight.specular;

	vec3 totalColor = vec3(0.0f);
	vec4 fragLightSpace = worldToLightTransform * vec4(fragment.position, 1.0f);
	totalColor += calcColorUnderSpotLight(fragment, light, viewDirection, fragLightSpace, lightDepthMap, castsShadows);

	OutColor = vec4(totalColor, 1.0f);
}

)