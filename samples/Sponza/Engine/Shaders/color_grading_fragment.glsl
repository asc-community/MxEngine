in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D tex;
uniform vec3 channelR;
uniform vec3 channelG;
uniform vec3 channelB;

void main()
{
    vec3 inputColor = texture(tex, TexCoord).rgb;
    vec3 outputColor = channelR * inputColor.r + channelG * inputColor.g + channelB * inputColor.b;
    
    outputColor = min(vec3(1), outputColor);

    OutColor = vec4(outputColor, 1.0f);
}