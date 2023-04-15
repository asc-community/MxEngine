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
    
    vec3 rayColor = vec3(1.f);

    const float maxSteps = 255.0f;
	const float sampleStep = 0.15f;
	float numOfStep = fragDistance/sampleStep;
	numOfStep = clamp(numOfStep,0.f,maxSteps);
	vec3 samplePosition = camera.position;
    float illum = 0.0f;
    for (float i = 0.0f; i < numOfStep; i++) 
	{
		samplePosition+=sampleStep*fragDirection;
		float shadowFactor = calcShadowFactorCascade(
            vec4(samplePosition,1.0), 
            lights[0],
            lightDepthMaps[0]);
        illum += smoothstep(0.0f, 1.0f, shadowFactor);
	}
    illum /= numOfStep;
    currentColor = mix(currentColor, rayColor, pow(illum, 0.7f));

    OutColor = vec4(currentColor,1.f);
}