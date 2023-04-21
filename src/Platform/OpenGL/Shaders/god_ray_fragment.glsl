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
		for (; i < maxSteps; i++) 
		{
			vec3 pos = camera.position + stp*fragDirection*(i+randomness);
			if(fragDistance<=distance(pos,camera.position))
				break;
			stp=stp*stepIncrement;
            
            float shadowFactor = calcShadowFactorCascade(
				vec4(pos,1.0), 
				lights[lightIndex],
				lightDepthMaps[lightIndex]);

			illum += clamp(shadowFactor,0.0f, 1.0f);
		}
		illum /= i;
		currentColor = mix(currentColor, lights[lightIndex].color.rgb/255.0f, illum);
    }

	OutColor = vec4(currentColor,1.f);
}