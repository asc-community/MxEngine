#include "Library/directional_light.glsl"

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

struct Camera
{
    vec3 position;
    mat4 invViewProjMatrix;
    mat4 viewProjMatrix;
};

uniform sampler2D cameraOutput;
uniform Camera camera;

uniform int lightCount;
uniform DirLight lights[MaxDirLightCount];
uniform sampler2D lightDepthMaps[MaxDirLightCount];

uniform float maxSteps;
uniform float sampleStep;
uniform float stepIncrement;

//sample only once without extra interpolation
float godRayShadowFactor2D(vec3 coords, sampler2D depthMap, vec4 textureLimitsXY, float bias)
{
    float compare = coords.z - bias;
    
    const float lod = getTextureLodLevel(coords.xy);
    vec2 texelSize = textureSize(depthMap, 0);
    vec2 texelSizeInv = 1.0 / texelSize;
    vec2 pixelPos = coords.xy * texelSize + vec2(0.5);
    vec2 fracPart = fract(pixelPos);
    vec2 startTexel = (pixelPos - fracPart) * texelSizeInv;
 	float res = sampleShadowMap(depthMap, startTexel + vec2( 0.0,  0.0) * texelSizeInv, lod, compare); 
	return pow(clamp(res, 0.0, 1.0), 8.0);
}
float godRayShadowSampler(vec3 position, DirLight light, sampler2D shadowMap)
{
	const float bias = 0.002;
	const vec2 textureSplitSize = vec2(1.01, 0.99) / DirLightCascadeMapCount;
	for(int cascadeIndex = 0; cascadeIndex<DirLightCascadeMapCount; cascadeIndex++)
	{
		float fIndex = float(cascadeIndex);
		vec4 textureLimitsXY = vec4(vec2(fIndex,fIndex + 1.f) * textureSplitSize, 0.001, 0.999);
		vec4 fragLightSpace = light.transform[cascadeIndex] * vec4(position,1.0);
		vec3 projectedPosition  = fragLightSpace.xyz / fragLightSpace.w;
		if (projectedPosition.x > textureLimitsXY[1] || projectedPosition.x < textureLimitsXY[0]||
			projectedPosition.y > textureLimitsXY[3] || projectedPosition.y < textureLimitsXY[2]||
			projectedPosition.z > 1.0 - bias || projectedPosition.z < bias)
		{
			continue;
		}
		return godRayShadowFactor2D(projectedPosition , shadowMap, textureLimitsXY, bias);
	}
	return 1.0f;
}

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    vec3 camera2Frag = fragment.position-camera.position;
    float fragDistance = length(camera2Frag);
    vec3 fragDirection = normalize(camera2Frag);
    vec3 currentColor = texture(cameraOutput, TexCoord).rgb; 
    
	
    float randomness = noise(TexCoord) * .6;//producing blur to decrease sampling rate
    for (int lightIndex = 0; lightIndex < lightCount; lightIndex++)
    {
        float illum = 0.0f;
		float i = 0.0f;
		float stp = sampleStep;
		vec3 pos = camera.position;
		for (; i < maxSteps; i++) 
		{
			pos = camera.position + stp*fragDirection*(i+randomness);
			if(fragDistance<=distance(pos,camera.position))
				break;
			stp*=stepIncrement;
			
			float shadowFactor=godRayShadowSampler(pos, lights[lightIndex], lightDepthMaps[lightIndex]);

			illum += clamp(shadowFactor,0.0f, 1.0f);
		}
		illum /= i;
		float distanceTraveled = length(camera.position-pos);
		illum *= distanceTraveled/15;
		illum = clamp(illum,0.0,0.7);
		currentColor = mix(currentColor, lights[lightIndex].color.rgb/80.0f, illum);
    }

	OutColor = vec4(currentColor,1.f);
}