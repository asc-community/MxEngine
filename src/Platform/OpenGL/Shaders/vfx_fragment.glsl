#include "shader_utils.glsl"
#include "fxaa.glsl"
#include "fog.glsl"
EMBEDDED_SHADER(

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

uniform sampler2D cameraOutput;
uniform Fog fog;
uniform bool enableFXAA;
uniform Camera camera;

void main()
{
	FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewMatrix, camera.invProjMatrix);
	float fragDistance = length(camera.position - fragment.position);
	
	vec3 currentColor;
	currentColor = enableFXAA ? fxaa(cameraOutput, TexCoord).rgb : texture(cameraOutput, TexCoord).rgb;
	currentColor = applyFog(currentColor, fragDistance, fog);

	OutColor = vec4(currentColor, 1.0f);
}

)