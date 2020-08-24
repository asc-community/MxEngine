#define MAKE_STRING(...) #__VA_ARGS__
MAKE_STRING(

out vec4 OutColor;
in vec2 TexCoord;

uniform sampler2D HDRTex;
uniform float exposure;
uniform float gamma;

void main()
{
	vec4 HDRColor = texture(HDRTex, TexCoord).rgba;
	vec3 gammaCorrectedColor = pow(HDRColor.rgb, vec3(1.0f / gamma));
	vec3 LDRColor = vec3(1.0f) - min(exp(-gammaCorrectedColor * exposure), 1.0f);
	OutColor = vec4(LDRColor.rgb, HDRColor.a);
}

)