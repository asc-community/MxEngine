EMBEDDED_SHADER(

vec3 getDisplacement(vec2 texCoord, sampler2D heightMap, float displacementFactor)
{
	if (texCoord.x < 0.001f || texCoord.y < 0.001f || texCoord.x > 0.999f || texCoord.y > 0.999f)
		return vec3(0.0f);
	vec3 heightTex = texture(heightMap, texCoord).rgb;
	return displacementFactor * heightTex;
}

)