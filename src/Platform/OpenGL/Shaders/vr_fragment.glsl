EMBEDDED_SHADER(

in vec2 TexCoord;
out vec4 Color;

uniform sampler2D leftEyeTex;
uniform sampler2D rightEyeTex;

void main()
{
    vec3 color = vec3(0.0f);
    vec2 projTexCoord = TexCoord;
    if (projTexCoord.x < 0.5f)
    {
        projTexCoord.x = projTexCoord.x * 2.0f;
        color = texture(leftEyeTex, projTexCoord).rgb;
    }
    else
    {
        projTexCoord.x = projTexCoord.x * 2.0f - 1.0f;
        color = texture(rightEyeTex, projTexCoord).rgb;
    }
    Color = vec4(color, 1.0f);
}

)