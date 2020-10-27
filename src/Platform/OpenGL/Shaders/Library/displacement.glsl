vec3 getDisplacement(vec2 texCoord, vec2 uvMultiplier, sampler2D heightMap, float displacementFactor)
{
	vec2 normTexCoord = texCoord / uvMultiplier;
	if (normTexCoord.x < 0.001f || normTexCoord.y < 0.001f ||
		normTexCoord.x > 0.999f || normTexCoord.y > 0.999f)
		return vec3(0.0f);

	vec3 heightTex = texture(heightMap, texCoord).rgb;
	return displacementFactor * heightTex;
}