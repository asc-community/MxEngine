in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D uImage;
uniform int uImageSamples;

void main()
{
    vec3 color = texture(uImage, TexCoord).rgb;
    color /= float(uImageSamples);
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));
    OutColor = vec4(color, 1.0);
}