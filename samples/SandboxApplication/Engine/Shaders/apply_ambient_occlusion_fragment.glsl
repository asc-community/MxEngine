in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D inputTex;
uniform sampler2D aoTex;
uniform float intensity;

void main()
{
    vec3 color = texture(inputTex, TexCoord).rgb;
    float occlusion = texture(aoTex, TexCoord).r;
    OutColor = vec4(color * pow(occlusion, intensity), 1.0);
}