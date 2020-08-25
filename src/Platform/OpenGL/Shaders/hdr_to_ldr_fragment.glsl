EMBEDDED_SHADER(

out vec4 OutColor;
in vec2 TexCoord;

uniform sampler2D HDRTex;
uniform float exposure;
uniform float gamma;

void main()
{
	vec4 HDRColor = texture(HDRTex, TexCoord).rgba;
	vec3 LDRColor = vec3(1.0f) - min(exp(-HDRColor.rgb * exposure), 1.0f);
	vec3 gammaCorrectedColor = pow(LDRColor.rgb, vec3(1.0f / gamma));
	OutColor = vec4(gammaCorrectedColor.rgb, HDRColor.a);
}

)