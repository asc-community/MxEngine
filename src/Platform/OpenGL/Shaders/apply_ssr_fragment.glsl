in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D SSRTex;
uniform sampler2D HDRTex;

void main()
{
    OutColor = vec4(texture(HDRTex, TexCoord));
}