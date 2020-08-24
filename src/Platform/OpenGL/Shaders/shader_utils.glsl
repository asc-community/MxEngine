#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

vec3 reconstructWorldPosition(float depth, vec2 texcoord, mat4 invProjection, mat4 invView)
{
	vec4 normPosition = vec4(2.0f * texcoord - vec2(1.0f), depth, 1.0f);
	vec4 viewSpacePosition = invProjection * normPosition;
	viewSpacePosition /= viewSpacePosition.w;
	vec4 worldPosition = invView * viewSpacePosition;
	return worldPosition.xyz;
}

float calcShadowFactor2D(vec4 fragPosLight, sampler2D depthMap, float bias, int blurIterations)
{
	vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
	float currentDepth = projCoords.z - bias;
	float shadowFactor = 0.0f;
	vec2 texelSize = 1.0f / textureSize(depthMap, 0);
	for (int x = -blurIterations; x <= blurIterations; x++)
	{
		for (int y = -blurIterations; y <= blurIterations; y++)
		{
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadowFactor += currentDepth > pcfDepth ? 0.0f : 1.0f;
		}
	}
	int iterations = 2 * blurIterations + 1;
	shadowFactor /= float(iterations * iterations);
	return shadowFactor;
}

const int POINT_LIGHT_SAMPLES = 20;
vec3 sampleOffsetDirections[POINT_LIGHT_SAMPLES] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float CalcShadowFactor3D(vec3 fragToLightRay, vec3 viewDist, float zfar, float bias, samplerCube depthMap)
{
	float invZfar = 1.0f / zfar;
	float currentDepth = length(fragToLightRay);
	currentDepth = (currentDepth - bias) * invZfar;
	float diskRadius = (1.0f + invZfar) * 0.04f;
	float shadowFactor = 0.0f;

	for (int i = 0; i < POINT_LIGHT_SAMPLES; i++)
	{
		float closestDepth = texture(depthMap, sampleOffsetDirections[i] * diskRadius - fragToLightRay).r;
		shadowFactor += (currentDepth < closestDepth) ? 1.0f : 0.0f;
	}
	shadowFactor /= float(POINT_LIGHT_SAMPLES);
	return shadowFactor;
}

vec3 calcReflectionColor(float reflectionFactor, samplerCube reflectionMap, mat3 reflectionMapTransform, vec3 viewDir, vec3 normal)
{
	vec3 I = -viewDir;
	vec3 reflectionRay = reflect(I, normal);
	reflectionRay = reflectionMapTransform * reflectionRay;
	vec3 color = reflectionFactor * texture(reflectionMap, reflectionRay).rgb;
	return color;
}

struct FragmentInfo
{
	vec3 albedo;
	float specularIntensity;
	float specularFactor;
	float emmisionFactor;
	float reflection;
	float depth;
	vec3 normal;
	vec3 position;
};

FragmentInfo getFragmentInfo(vec2 texCoord, sampler2D albedoTexture, sampler2D normalTexture, sampler2D materialTexture, sampler2D depthTexture, mat4 invView, mat4 invProjection)
{
	FragmentInfo fragment;

	fragment.albedo = texture(albedoTexture, texCoord).rgb;
	fragment.normal = 2.0f * texture(normalTexture, texCoord).rgb - vec3(1.0f);
	vec4 material = texture(materialTexture, texCoord).rgba;
	fragment.depth = texture(depthTexture, texCoord).r;

	fragment.emmisionFactor = material.r;
	fragment.reflection = material.g;
	fragment.specularIntensity = 1.0f / material.b;
	fragment.specularFactor = material.a;

	fragment.position = reconstructWorldPosition(fragment.depth, texCoord, invProjection, invView);

	return fragment;
}

)