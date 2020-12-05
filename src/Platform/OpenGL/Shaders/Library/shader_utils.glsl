vec3 reconstructWorldPosition(float depth, vec2 texcoord, mat4 invViewProjMatrix)
{
	vec4 normPosition = vec4(2.0f * texcoord - vec2(1.0f), depth, 1.0f);
	vec4 worldPosition = invViewProjMatrix * normPosition;
	worldPosition /= worldPosition.w;
	return worldPosition.xyz;
}

float calcShadowFactor2D(vec4 fragPosLight, sampler2D depthMap, float bias, int blurIterations)
{
	vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
	if (projCoords.z > 0.99f) return 1.0f; // do not handle corner cases, assume now shadows
	float currentDepth = projCoords.z - bias;
	float shadowFactor = 0.0f;
	vec2 texelSize = 1.0f / textureSize(depthMap, 0);
	for (int x = -blurIterations; x <= blurIterations; x++)
	{
		for (int y = -blurIterations; y <= blurIterations; y++)
		{
			float pcfDepth = textureLod(depthMap, projCoords.xy + vec2(x, y) * texelSize, 0).r;
			shadowFactor += (currentDepth > pcfDepth) ? 0.0f : 1.0f;
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
		float closestDepth = textureLod(depthMap, sampleOffsetDirections[i] * diskRadius - fragToLightRay, 0).r;
		shadowFactor += (currentDepth > closestDepth) ? 0.0f : 1.0f;
	}
	shadowFactor /= float(POINT_LIGHT_SAMPLES);
	return shadowFactor;
}

vec3 calcReflectionColor(samplerCube reflectionMap, mat3 reflectionMapTransform, vec3 viewDir, vec3 normal)
{
	vec3 I = -viewDir;
	vec3 reflectionRay = reflect(I, normal);
	reflectionRay = reflectionMapTransform * reflectionRay;
	vec3 color = texture(reflectionMap, reflectionRay).rgb;
	return color;
}

vec4 worldToFragSpace(vec3 v, mat4 viewProj)
{
	vec4 proj = viewProj * vec4(v, 1.0f);
	proj.xyz /= proj.w;
	proj.xy = proj.xy * 0.5f + vec2(0.5f);
	return proj;
}

struct FragmentInfo
{
	vec3 albedo;
	float ambientOcclusion;
	float emmisionFactor;
	float roughnessFactor;
	float metallicFactor;
	float depth;
	vec3 normal;
	vec3 position;
};

struct EnvironmentInfo
{
	samplerCube skybox;
	samplerCube irradiance;
	mat3 skyboxRotation;
	float intensity;
};

FragmentInfo getFragmentInfo(vec2 texCoord, sampler2D albedoTexture, sampler2D normalTexture, sampler2D materialTexture, sampler2D depthTexture, mat4 invViewProjMatrix)
{
	FragmentInfo fragment;

	fragment.normal = normalize(texture(normalTexture, texCoord).rgb - vec3(0.5f));
	vec4 albedo = texture(albedoTexture, texCoord).rgba;
	vec4 material = texture(materialTexture, texCoord).rgba;
	fragment.depth = texture(depthTexture, texCoord).r;

	fragment.albedo = albedo.rgb;
	fragment.ambientOcclusion = albedo.a;
	fragment.emmisionFactor = material.r / (1.0f - material.r);
	fragment.roughnessFactor = material.g;
	fragment.metallicFactor = material.b;

	fragment.position = reconstructWorldPosition(fragment.depth, texCoord, invViewProjMatrix);

	return fragment;
}