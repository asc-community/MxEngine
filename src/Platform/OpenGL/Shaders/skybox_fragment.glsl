#define MAKE_STRING(...) #__VA_ARGS__

#include "fog.glsl"
MAKE_STRING(

out vec4 Color;
in vec3 TexCoords;

uniform samplerCube skybox;
uniform float gamma;
uniform Fog fog;

void main()
{
	vec3 skyboxColor = texture(skybox, TexCoords).rgb;
	skyboxColor = pow(skyboxColor, vec3(gamma));
	Color = vec4(applySkyFog(skyboxColor, fog), 1.0f);
}

)