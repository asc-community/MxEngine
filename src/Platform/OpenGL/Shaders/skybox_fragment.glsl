#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

out vec4 Color;
in vec3 TexCoords;

struct Fog
{
	float distance;
	float density;
	vec3 color;
};

uniform samplerCube skybox;
uniform Fog fog;

void main()
{
	vec3 skyboxColor = texture(skybox, TexCoords).rgb;
	float fogFactor = 1.0f - fog.distance / (1.0f + fog.density);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	Color = vec4(mix(skyboxColor, fog.color, fogFactor), 1.0f);
}

)