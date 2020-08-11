#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

out vec4 OutColor;
in vec2 TexCoord;

uniform sampler2D HDRTex;
uniform float exposure;

void main()
{
	vec4 HDRColor = texture(HDRTex, TexCoord).rgba;
	vec3 LDRColor = vec3(1.0f) - exp(-HDRColor.rgb * exposure);
	OutColor = vec4(LDRColor.rgb, HDRColor.a);
}

)