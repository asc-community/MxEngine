#include "Library/directional_light.glsl"

out vec4 OutColor;
in vec2 TexCoord;

struct Camera
{
	vec3 position;
	mat4 invViewProjMatrix;
	mat4 viewProjMatrix;
};

uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D depthTex;

uniform int lightCount;
uniform int pcfDistance;
uniform Camera camera;

uniform EnvironmentInfo environment;

const int MaxLightCount = 4;
uniform DirLight lights[MaxLightCount];
uniform sampler2D lightDepthMaps[MaxLightCount][DirLightCascadeMapCount];

void main()
{
	FragmentInfo fragment = getFragmentInfo(TexCoord, albedoTex, normalTex, materialTex, depthTex, camera.invViewProjMatrix);
	float fragDistance = length(camera.position - fragment.position);
	
	vec3 viewDirection = normalize(camera.position - fragment.position);
	
	vec3 totalColor = 0.0001f * fragment.albedo;
	for (int i = 0; i < lightCount; i++)
	{
		float shadowFactor = calcShadowFactorCascade(vec4(fragment.position, 1.0f), lights[i], lightDepthMaps[i], pcfDistance);
		totalColor += calcColorUnderDirLight(fragment, lights[i], viewDirection, shadowFactor, environment);
	}
	
	OutColor = vec4(totalColor, 1.0f);
}