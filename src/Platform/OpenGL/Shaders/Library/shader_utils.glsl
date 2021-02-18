vec3 reconstructWorldPosition(float depth, vec2 texcoord, mat4 invViewProjMatrix)
{
	vec4 normPosition = vec4(2.0f * texcoord - vec2(1.0f), depth, 1.0f);
	vec4 worldPosition = invViewProjMatrix * normPosition;
	worldPosition /= worldPosition.w;
	return worldPosition.xyz;
}

float sampleShadowMap(sampler2D depthMap, vec2 coords, float compare)
{
	return step(compare, textureLod(depthMap, coords, 0.0).r);
}

float sampleShadowMapLinear(sampler2D depthMap, vec2 coords, float compare, vec2 texelSize)
{
	vec2 pixelPos = coords / texelSize + vec2(0.5);
	vec2 fracPart = fract(pixelPos);
	vec2 startTexel = (pixelPos - fracPart) * texelSize;

	vec4 samples;
	samples[0] = sampleShadowMap(depthMap, startTexel + vec2(0.0, 0.0) * texelSize, compare);
	samples[1] = sampleShadowMap(depthMap, startTexel + vec2(1.0, 0.0) * texelSize, compare);
	samples[2] = sampleShadowMap(depthMap, startTexel + vec2(0.0, 1.0) * texelSize, compare);
	samples[3] = sampleShadowMap(depthMap, startTexel + vec2(1.0, 1.0) * texelSize, compare);

	float sampleA = mix(samples[0], samples[2], fracPart.y);
	float sampleB = mix(samples[1], samples[3], fracPart.y);
	return mix(sampleA, sampleB, fracPart.x);
}

float calcShadowFactor2D(vec3 projCoords, sampler2D depthMap, float bias, int blurIterations)
{
	if (projCoords.z > 0.999 || projCoords.z < 0.001) return 1.0; // do not handle corner cases, assume now shadows
	float currentDepth = projCoords.z - bias;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	
	float sample1 = sampleShadowMapLinear(depthMap, projCoords.xy - texelSize, currentDepth, texelSize);
	float sample2 = sampleShadowMapLinear(depthMap, projCoords.xy + texelSize, currentDepth, texelSize);
	return mix(sample1, sample2, 0.5);
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

float getTextureLodLevel(vec2 uv)
{
	vec2 dx_vtc = dFdx(uv);
	vec2 dy_vtc = dFdy(uv);
	float delta_max_sqr = max(dot(dx_vtc, dx_vtc), dot(dy_vtc, dy_vtc));
	return 0.5 * log2(delta_max_sqr);
}

vec3 calcReflectionColor(samplerCube reflectionMap, mat3 reflectionMapTransform, vec3 viewDir, vec3 normal, float lod)
{
	vec3 I = -viewDir;
	vec3 reflectionRay = reflect(I, normal);
	reflectionRay = dot(viewDir, normal) < 0.0 ? -reflectionRay : reflectionRay;
	reflectionRay = reflectionMapTransform * reflectionRay;

	float defaultLod = getTextureLodLevel(reflectionRay.xy);
	vec3 color = textureLod(reflectionMap, reflectionRay, max(lod, defaultLod)).rgb;
	return color;
}

vec3 calcReflectionColor(samplerCube reflectionMap, mat3 reflectionMapTransform, vec3 viewDir, vec3 normal)
{
	return calcReflectionColor(reflectionMap, reflectionMapTransform, viewDir, normal, 0.0);
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