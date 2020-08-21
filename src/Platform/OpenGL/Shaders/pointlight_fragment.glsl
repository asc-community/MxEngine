#define MAKE_STRING(...) #__VA_ARGS__

#include "shader_utils.glsl"
MAKE_STRING(

out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

struct PointLight
{
	vec3 position;
	float radius;
	float zfar;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform PointLight pointLight;
uniform samplerCube lightDepthMap;

uniform int pcfDistance;
uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;
uniform vec3 viewPosition;
uniform vec2 viewportSize;

vec3 calcColorUnderPointLight(vec3 albedo, float specularIntensity, float specularFactor, vec3 normal, vec3 fragWorldSpace, PointLight light, vec3 viewDir, samplerCube map_shadow)
{
	vec3 lightPath = light.position - fragWorldSpace;
	float lightDistance = length(lightPath);

	if (light.radius < lightDistance || isnan(lightDistance))
		discard;

	vec3 lightDir = normalize(lightPath);
	vec3 Hdir = normalize(lightDir + viewDir);
	float shadowFactor = CalcShadowFactor3D(lightPath, viewDir, light.zfar, 0.15f, map_shadow);

	float intensity = (light.radius - lightDistance) / light.radius;

	float diffuseCoef = max(dot(lightDir, normal), 0.0f);
	float specularCoef = pow(clamp(dot(Hdir, normal), 0.0f, 1.0f), specularIntensity);

	vec3 ambientColor = albedo;
	vec3 diffuseColor = albedo * diffuseCoef;
	vec3 specularColor = vec3(specularCoef * specularFactor);

	ambientColor  = ambientColor  * intensity * light.ambient;
	diffuseColor  = diffuseColor  * intensity * light.diffuse;
	specularColor = specularColor * intensity * light.specular;

	return vec3(ambientColor + shadowFactor * (diffuseColor + specularColor));
}

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / viewportSize;

	vec3 albedo = texture(albedoTex, TexCoord).rgb;
	vec3 normal = 2.0f * texture(normalTex, TexCoord).rgb - vec3(1.0f);
	vec4 material = texture(materialTex, TexCoord).rgba;
	float depth = texture(depthTex, TexCoord).r;

	float emmisionFactor = material.r;
	float reflection = material.g;
	float specularIntensity = 1.0f / material.b;
	float specularFactor = material.a;

	vec3 fragPosition = reconstructWorldPosition(depth, TexCoord, invProjMatrix, invViewMatrix);
	float fragDistance = length(viewPosition - fragPosition);
	vec3 viewDirection = normalize(viewPosition - fragPosition);

	vec3 totalColor = vec3(0.0f);
	totalColor += calcColorUnderPointLight(albedo, specularIntensity, specularFactor, normal, fragPosition, pointLight, viewDirection, lightDepthMap);

	OutColor = vec4(totalColor, 1.0f);
}

)