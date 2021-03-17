in vec2 TexCoord;

out vec4 Color;

uniform sampler2D inputTex;
uniform bool horizontalBlur;
uniform int lod;

float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texOffset = 1.0 / textureSize(inputTex, lod);
    vec3 color = texture(inputTex, TexCoord, lod).rgb * weights[0];
    vec2 direction = horizontalBlur ? vec2(texOffset.x, 0.0) : vec2(0.0, texOffset.y);

    for (int i = 1; i < 5; i++)
    {
        color += texture(inputTex, TexCoord + direction * i, lod).rgb * weights[i];
        color += texture(inputTex, TexCoord - direction * i, lod).rgb * weights[i];
    }
    Color = vec4(color, 1.0);
}