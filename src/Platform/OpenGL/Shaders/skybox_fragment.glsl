#include "fog.glsl"
EMBEDDED_SHADER(

out vec4 Color;
in vec3 TexCoords;

uniform samplerCube skybox;
uniform float gamma;
uniform float luminance;
uniform Fog fog;

void main()
{
	vec3 skyboxColor = texture(skybox, TexCoords).rgb;
	skyboxColor = pow(skyboxColor, vec3(gamma));
	skyboxColor = luminance * skyboxColor;
	skyboxColor = applySkyFog(skyboxColor, fog);
	Color = vec4(skyboxColor, 1.0f);
}

)