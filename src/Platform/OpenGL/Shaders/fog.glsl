EMBEDDED_SHADER(
	
struct Fog
{
	float distance;
	float density;
	vec3 color;
};

vec3 applyFog(vec3 color, float fragDistance, Fog fog)
{
	float fogFactor = 1.0f - fog.distance * exp(-fragDistance * fog.density);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	return mix(color, fog.color, fogFactor);
}

vec3 applySkyFog(vec3 color, Fog fog)
{
	float fogFactor = 1.0f - fog.distance * exp(-10.0f * fog.density);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	return mix(color, fog.color, fogFactor);
}

)