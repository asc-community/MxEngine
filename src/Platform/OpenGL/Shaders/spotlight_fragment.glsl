#define MAKE_STRING(...) #__VA_ARGS__

#include "shader_utils.glsl"
MAKE_STRING(

out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

struct SpotLight
{
	mat4 transform;
	vec3 position;
	vec3 direction;
	float innerAngle;
	float outerAngle;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform SpotLight spotLight;
uniform sampler2D lightDepthMap;

uniform int pcfDistance;
uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;
uniform vec3 viewPosition;
uniform vec2 viewportSize;

vec3 calcColorUnderSpotLight(vec3 albedo, float specularIntensity, float specularFactor, vec3 normal, vec3 fragWorldSpace, SpotLight light, vec3 viewDir, vec4 fragLightSpace, sampler2D map_shadow)
{
	vec3 lightDir = normalize(light.position - fragWorldSpace);
	vec3 Hdir = normalize(lightDir + viewDir);
	float shadowFactor = calcShadowFactor2D(fragLightSpace, map_shadow, 0.005f, pcfDistance);

	float diffuseCoef = max(dot(lightDir, normal), 0.0f);
	float specularCoef = pow(clamp(dot(Hdir, normal), 0.0f, 1.0f), specularIntensity);

	float fragAngle = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerAngle - light.outerAngle;
	float intensity = clamp((fragAngle - light.outerAngle) / epsilon, 0.0f, 1.0f);

	vec3 ambientColor  = albedo;
	vec3 diffuseColor  = albedo * diffuseCoef;
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

	vec4 fragLightSpace = spotLight.transform * vec4(fragPosition, 1.0f);
	totalColor += calcColorUnderSpotLight(albedo, specularIntensity, specularFactor, normal, fragPosition, spotLight, viewDirection, fragLightSpace, lightDepthMap);

	OutColor = vec4(totalColor, 1.0f);
}

)