#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(
	
struct Fog
{
	float distance;
	float density;
	vec3 color;
};

vec3 applyFog(vec3 color, float fragDistance, vec3 viewDir, Fog fog)
{
	float fogFactor = 1.0f - fog.distance * exp(-fragDistance * fog.density);
	return mix(color, fog.color, clamp(fogFactor, 0.0f, 1.0f));
}

vec3 applySkyFog(vec3 color, Fog fog)
{
	float fogFactor = 1.0f - fog.distance / (1.0f + fog.density);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	return mix(color, fog.color, fogFactor);
}

)