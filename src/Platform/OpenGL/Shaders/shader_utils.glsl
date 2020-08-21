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

vec3 applyFog(vec3 color, float distance, vec3 viewDir, float fogDensity, float fogDistance, vec3 fogColor)
{
	float fogFactor = 1.0f - fogDistance * exp(-distance * fogDensity);
	return mix(color, fogColor, clamp(fogFactor, 0.0f, 1.0f));
}

vec3 calcReflectionColor(float reflectionFactor, samplerCube reflectionMap, vec2 texcoord, mat3 reflectionMapTransform, vec3 viewDir, vec3 normal)
{
	vec3 I = -viewDir;
	vec3 reflectionRay = reflect(I, normal);
	reflectionRay = reflectionMapTransform * reflectionRay;
	vec3 color = reflectionFactor * texture(reflectionMap, reflectionRay).rgb;
	return color;
}

)