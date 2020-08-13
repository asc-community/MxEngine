#define MAKE_STRING(...) #__VA_ARGS__
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

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

uniform int lightCount;
uniform int pcfDistance;
uniform float fogDistance;
uniform float fogDensity;
uniform vec3 fogColor;
uniform bool isViewPerspective;
uniform vec3 viewPosition;
uniform mat4 invViewMatrix;
uniform mat4 invProjMatrix;
uniform mat3 skyboxTransform;
uniform samplerCube skyboxTex;

const int MaxLightCount = 2;
uniform DirLight lights[MaxLightCount];
uniform sampler2D lightDepthMaps[MaxLightCount];

vec3 reconstructWorldPosition(float depth, vec2 texcoord)
{
	vec4 normPosition = vec4(2.0f * texcoord - vec2(1.0f), depth, 1.0f);
	vec4 viewSpacePosition = invProjMatrix * normPosition;
	viewSpacePosition /= viewSpacePosition.w;
	vec4 worldPosition = invViewMatrix * viewSpacePosition;
	return worldPosition.xyz;
}

vec3 calcReflectionColor(float reflectionFactor, samplerCube map_reflection, vec2 texcoord, mat3 reflectionMapTransform, vec3 viewDir, vec3 normal)
{
	vec3 I = -viewDir;
	vec3 reflectionRay = reflect(I, normal);
	reflectionRay = reflectionMapTransform * reflectionRay;
	vec3 color = reflectionFactor * texture(map_reflection, reflectionRay).rgb;
	return color;
}

float calcShadowFactor(vec4 fragPosLight, sampler2D map_shadow, float bias)
{
	vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
	float currentDepth = projCoords.z - bias;
	float shadowFactor = 0.0f;
	vec2 texelSize = 1.0f / textureSize(map_shadow, 0);
	for (int x = -pcfDistance; x <= pcfDistance; x++)
	{
		for (int y = -pcfDistance; y <= pcfDistance; y++)
		{
			float pcfDepth = texture(map_shadow, projCoords.xy + vec2(x, y) * texelSize).r;
			shadowFactor += currentDepth > pcfDepth ? 0.0f : 1.0f;
		}
	}
	int iterations = 2 * pcfDistance + 1;
	shadowFactor /= float(iterations * iterations);
	return shadowFactor;
}

vec3 calcColorUnderDirLight(vec3 albedo, float specularIntensity, float specularFactor, float reflectionFactor, vec3 reflectionColor, vec3 normal, DirLight light, vec3 viewDir, vec4 fragLightSpace, sampler2D map_shadow)
{
	vec3 lightDir = normalize(light.direction);
	vec3 Hdir = normalize(lightDir + viewDir);
	float shadowFactor = calcShadowFactor(fragLightSpace, map_shadow, 0.005f);

	float diffuseCoef = max(dot(lightDir, normal), 0.0f);
	float specularCoef = pow(clamp(dot(Hdir, normal), 0.0f, 1.0f), specularIntensity);

	vec3 ambientColor = albedo;
	vec3 diffuseColor = albedo * diffuseCoef;
	vec3 specularColor = vec3(specularCoef * specularFactor);
	reflectionColor = reflectionColor * (diffuseColor + ambientColor);

	ambientColor = ambientColor * light.ambient;
	diffuseColor = diffuseColor * light.diffuse;
	specularColor = specularColor * light.specular;

	return vec3(ambientColor + specularColor + shadowFactor * mix(diffuseColor, reflectionColor, reflectionFactor));
}

vec3 applyFog(vec3 color, float distance, vec3 viewDir)
{
	float fogFactor = 1.0f - fogDistance * exp(-distance * fogDensity);
	return mix(color, fogColor, clamp(fogFactor, 0.0f, 1.0f));
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

	vec3 fragPosition = reconstructWorldPosition(depth, TexCoord);
	float fragDistance = length(viewPosition - fragPosition.xyz);
	vec3 viewDirection = normalize(viewPosition - fragPosition.xyz);
	vec3 reflectionColor = calcReflectionColor(reflection, skyboxTex, TexCoord, skyboxTransform, viewDirection, normal);

	vec3 totalColor = vec3(0.0f);
	for (int i = 0; i < lightCount; i++)
	{
		vec4 fragLightSpace = lights[i].transform * vec4(fragPosition, 1.0f);
		totalColor += calcColorUnderDirLight(albedo, specularIntensity, specularFactor, reflection, reflectionColor, normal, lights[i], viewDirection, fragLightSpace, lightDepthMaps[i]);
	}

	totalColor = applyFog(totalColor, fragDistance, viewDirection);

	OutColor = vec4(totalColor, 1.0f);
}

)