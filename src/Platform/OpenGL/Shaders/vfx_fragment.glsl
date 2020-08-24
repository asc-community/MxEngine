#define MAKE_STRING(...) #__VA_ARGS__

#include "shader_utils.glsl"
#include "fxaa.glsl"
MAKE_STRING(

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

struct Camera
{
	vec3 position;
	mat4 invProjMatrix;
	mat4 invViewMatrix;
	mat4 viewProjMatrix;
};

struct Fog
{
	vec3 color;
	float density;
	float distance;
};

uniform sampler2D cameraOutput;
uniform Fog fog;
uniform bool enableFXAA;
uniform Camera camera;

void main()
{
	FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewMatrix, camera.invProjMatrix);
	vec3 viewDirection = normalize(camera.position - fragment.position);
	float fragDistance = length(camera.position - fragment.position);
	
	vec3 currentColor;
	currentColor = enableFXAA ? fxaa(cameraOutput, TexCoord).rgb : texture(cameraOutput, TexCoord).rgb;
	currentColor = applyFog(currentColor, fragDistance, viewDirection, fog.density, fog.distance, fog.color);

	OutColor = vec4(currentColor, 1.0f);
}

)