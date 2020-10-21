struct Fog
{
	float distance;
	float density;
	vec3 color;
};

vec3 applyFog(vec3 color, float fragDistance, Fog fog)
{
	//if (fragDistance >= 10000.0f) fragDistance = 10.0f; // used for sky fog
	float fogFactor = 1.0f - fog.distance * exp(-fragDistance * fog.density);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	return mix(color, fog.color, fogFactor);
}