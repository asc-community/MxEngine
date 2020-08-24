#define MAKE_STRING(...) #__VA_ARGS__

#include "shader_utils.glsl"
MAKE_STRING(

out vec4 OutColor;
in vec2 TexCoord;

struct DirLight
{
	mat4 transform;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

struct Fog
{
	vec3 color;
	float density;
	float distance;
};

struct Camera
{
	vec3 position;
	mat4 invProjMatrix;
	mat4 invViewMatrix;
	mat4 viewProjMatrix;
};

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

uniform int lightCount;
uniform int pcfDistance;
uniform Fog fog;
uniform Camera camera;
uniform mat3 skyboxTransform;
uniform samplerCube skyboxTex;

const int MaxLightCount = 4;
uniform DirLight lights[MaxLightCount];
uniform sampler2D lightDepthMaps[MaxLightCount];

vec3 calcColorUnderDirLight(vec3 albedo, float specularIntensity, float specularFactor, float reflectionFactor, vec3 reflectionColor, vec3 normal, DirLight light, vec3 viewDir, vec4 fragLightSpace, sampler2D map_shadow)
{
	vec3 lightDir = normalize(light.direction);
	vec3 Hdir = normalize(lightDir + viewDir);
	float shadowFactor = calcShadowFactor2D(fragLightSpace, map_shadow, 0.005f, pcfDistance);

	float diffuseCoef = max(dot(lightDir, normal), 0.0f);
	float specularCoef = pow(clamp(dot(Hdir, normal), 0.0f, 1.0f), specularIntensity);

	vec3 ambientColor = albedo;
	vec3 diffuseColor = albedo * diffuseCoef;
	vec3 specularColor = vec3(specularCoef * specularFactor);
	reflectionColor = reflectionColor * (diffuseColor + ambientColor);

	ambientColor = ambientColor * light.ambient;
	diffuseColor = diffuseColor * light.diffuse;
	specularColor = specularColor * light.specular;

	return vec3(ambientColor + shadowFactor * mix(diffuseColor + specularColor, reflectionColor, reflectionFactor));
}

void main()
{
	vec3 albedo = texture(albedoTex, TexCoord).rgb;
	vec3 normal = 2.0f * texture(normalTex, TexCoord).rgb - vec3(1.0f);
	vec4 material = texture(materialTex, TexCoord).rgba;
	float depth = texture(depthTex, TexCoord).r;

	float emmisionFactor = material.r;
	float reflection = material.g;
	float specularIntensity = 1.0f / material.b;
	float specularFactor = material.a;

	vec3 fragPosition = reconstructWorldPosition(depth, TexCoord, camera.invProjMatrix, camera.invViewMatrix);
	float fragDistance = length(camera.position - fragPosition.xyz);
	vec3 viewDirection = normalize(camera.position - fragPosition.xyz);
	vec3 reflectionColor = calcReflectionColor(reflection, skyboxTex, TexCoord, skyboxTransform, viewDirection, normal);

	vec3 totalColor = vec3(0.0f);
	for (int i = 0; i < lightCount; i++)
	{
		vec4 fragLightSpace = lights[i].transform * vec4(fragPosition, 1.0f);
		totalColor += calcColorUnderDirLight(albedo, specularIntensity, specularFactor, reflection, reflectionColor, normal, lights[i], viewDirection, fragLightSpace, lightDepthMaps[i]);
	}

	OutColor = vec4(totalColor, 1.0f);
}

)