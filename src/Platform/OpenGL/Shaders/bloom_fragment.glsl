#define MAKE_STRING(...) #__VA_ARGS__
R"(
#version 400 core
)" \
MAKE_STRING(

in vec2 TexCoord;

out vec4 Color;

uniform sampler2D BloomTexture;
uniform bool horizontalKernel;
uniform float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texOffset = 1.0 / textureSize(BloomTexture, 0);
    vec3 color = texture(BloomTexture, TexCoord).rgb * weights[0];
    if (horizontalKernel)
    {
        for (int i = 1; i < 5; ++i)
        {
            color += texture(BloomTexture, TexCoord + vec2(texOffset.x * i, 0.0)).rgb * weights[i];
            color += texture(BloomTexture, TexCoord - vec2(texOffset.x * i, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            color += texture(BloomTexture, TexCoord + vec2(0.0, texOffset.y * i)).rgb * weights[i];
            color += texture(BloomTexture, TexCoord - vec2(0.0, texOffset.y * i)).rgb * weights[i];
        }
    }
    Color = vec4(color, 1.0);
}
)