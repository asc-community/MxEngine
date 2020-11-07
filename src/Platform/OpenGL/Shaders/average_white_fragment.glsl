in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D curFrameHDR;
uniform sampler2D prevFrameWhite;
uniform float eyeAdaptation;

vec3 luminance = vec3(0.2125f, 0.7154f, 0.0721f);

void main()
{
    vec3 color = texture(curFrameHDR, TexCoord).rgb;
    float oldWhite = texture(prevFrameWhite, vec2(0.0f)).r;
    float curWhite = dot(luminance, color);
    float white = oldWhite + (curWhite - oldWhite) * eyeAdaptation;
    OutColor = vec4(white, 0.0f, 0.0f, 1.0f);
}