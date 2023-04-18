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

void main()
{
    FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
    vec3 camera2Frag = fragment.position-camera.position;
    float fragDistance = length(camera2Frag);
    vec3 fragDirection = normalize(camera2Frag);
    vec3 currentColor = texture(cameraOutput, TexCoord).rgb; 
    
    const float maxSteps =  255.0f;
	const float sampleStep = 1.5f;
	float numOfStep = fragDistance/sampleStep;
	numOfStep = clamp(numOfStep,0.f,maxSteps);
	
    float randomness = noise(TexCoord) * .6;//producing blur to decrease sampling rate
    for (int lightIndex = 0; lightIndex < lightCount; lightIndex++)
    {
		float illum = 0.0f;
		for (float i = 0.0f; i < numOfStep; i++) 
		{
			vec3 pos = camera.position + sampleStep*fragDirection*(i+randomness );
			float shadowFactor = calcShadowFactorCascade(
				vec4(pos,1.0), 
				lights[lightIndex],
				lightDepthMaps[lightIndex]);
			illum += smoothstep(0.0f, 1.0f, shadowFactor);
		}
		illum /= numOfStep;
		currentColor = mix(currentColor, lights[lightIndex].color.rgb/255.0f, pow(illum, 0.7f));
    }

	OutColor = vec4(currentColor,1.f);
}